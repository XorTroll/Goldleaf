using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;
using System.Windows.Forms;
using libusbK;

namespace gtree
{
    public enum CommandId : uint
    {
        GetDriveCount,
        GetDriveInfo,
        StatPath,
        GetFileCount,
        GetFile,
        GetDirectoryCount,
        GetDirectory,
        ReadFile,
        WriteFile,
        Create,
        Delete,
        Rename,
        GetSpecialPathCount,
        GetSpecialPath,
        SelectFile,
        Max,
    }

    public static class StringExtras
    {
        public static string NormalizeAsPath(this string Str)
        {
            return Str.Replace('/', '\\');
        }

        public static string NormalizeAsGoldleafPath(this string Str)
        {
            return Str.Replace('\\', '/');
        }
    }

    public static class USBCommands
    {
        private const int CommandBlockLength = 0x1000;
        private static byte[] CommandBlockBuf = new byte[CommandBlockLength];

        private static readonly byte ReadPipeId = 0x81;
        private static readonly byte WritePipeId = 0x1;

        private static BinaryReader LastReadFile = null;
        private static string LastRFile = null;

        private static FileStream LastWriteFile = null;
        private static string LastWFile = null;

        private static string LastDirectoriesPath = null;
        private static string[] LastDirectories = null;

        private static string LastFilesPath = null;
        private static string[] LastFiles = null;

        private static List<DriveInfo> LastDrives = null;

        private static readonly Dictionary<string, Environment.SpecialFolder> SpecialPathList = new Dictionary<string, Environment.SpecialFolder>()
        {
            { "Desktop", Environment.SpecialFolder.Desktop },
            { "Documents", Environment.SpecialFolder.MyDocuments },
        };

        public static string ReadStringEx(this BinaryReader Reader)
        {
            var strlen = Reader.ReadUInt32();
            var str = Reader.ReadBytes((int)strlen);
            return Encoding.UTF8.GetString(str).Replace("\0", "");
        }

        public static void WriteStringEx(this BinaryWriter Writer, string Str)
        {
            Writer.Write((uint)Str.Length);
            Writer.Write(Encoding.UTF8.GetBytes(Str));
        }

        public static byte[] ReadBuffer(this UsbK USB, ulong Size)
        {
            var data = new byte[Size];
            USB.ReadPipe(ReadPipeId, data, (int)Size, out int _, IntPtr.Zero);
            return data;
        }

        public static void WriteBuffer(this UsbK USB, byte[] Buf)
        {
            USB.WritePipe(WritePipeId, Buf, Buf.Length, out int _, IntPtr.Zero);
        }

        public static void CommandHandleLoop(this UsbK USB)
        {
            Array.Clear(CommandBlockBuf, 0, CommandBlockLength);
            USB.ReadPipe(ReadPipeId, CommandBlockBuf, CommandBlockLength, out int readbuf, IntPtr.Zero);
            if(readbuf == 0) return;
            if(readbuf != CommandBlockLength)
            {
                Program.Warn.LogL($"Command block read size mismatch (expected: 0x{CommandBlockLength:X}, got 0x{readbuf:X}!)");
                return;
            }
            using(var strm = new MemoryStream(CommandBlockBuf))
            {
                using(var inblock = new BinaryReader(strm))
                {
                    var inmagic = inblock.ReadUInt32();
                    if(inmagic != Command.InputMagic)
                    {
                        Program.Warn.LogL($"Input magic mismatch (expected: 0x{Command.InputMagic:X}, got 0x{inmagic:X}!)");
                        return;
                    }
                    var command = inblock.ReadUInt32();
                    if (!Enum.IsDefined(typeof(CommandId), command))
                    {
                        Program.Warn.LogL($"Invalid command Id (got {command}!)");
                        return;
                    }
                    
                    CommandId cmd = (CommandId)command;
                    Program.Log.LogL("Received command: " + cmd.ToString());

                    var outblockbuf = new byte[CommandBlockLength];
                    var bufs = new List<byte[]>();

                    using(var wstrm = new MemoryStream(outblockbuf))
                    {
                        using(var outblock = new BinaryWriter(wstrm))
                        {
                            bool out_written = false;

                            void WriteOutBlockBase(uint Result)
                            {
                                if(out_written) return;
                                out_written = true;
                                outblock.Write(Command.OutputMagic);
                                outblock.Write(Result);
                            }

                            switch(cmd)
                            {
                                case CommandId.GetDriveCount:
                                    {
                                        var drives = DriveInfo.GetDrives().Where(drive => drive.IsReady).ToList();
                                        LastDrives = drives;
                                        WriteOutBlockBase(Command.ResultSuccess);
                                        outblock.Write((uint)drives.Count);
                                        break;
                                    }
                                case CommandId.GetDriveInfo:
                                    {
                                        var driveidx = inblock.ReadUInt32();
                                        var drives = LastDrives;
                                        if(LastDrives == null)
                                        {
                                            drives = DriveInfo.GetDrives().Where(drive => drive.IsReady).ToList();
                                            LastDrives = drives;
                                        }
                                        if(driveidx >= drives.Count) WriteOutBlockBase(Command.ResultInvalidInput);
                                        else
                                        {
                                            var drive = drives[(int)driveidx];
                                            WriteOutBlockBase(Command.ResultSuccess);
                                            string label = drive.VolumeLabel;
                                            string prefix = drive.Name.Substring(0, 1);
                                            outblock.WriteStringEx(label);
                                            outblock.WriteStringEx(prefix);
                                            outblock.Write((uint)drive.TotalFreeSpace);
                                            outblock.Write((uint)drive.TotalSize);
                                        }
                                        break;
                                    }
                                case CommandId.StatPath:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        uint type = 0;
                                        ulong filesize = 0;
                                        if(File.Exists(path))
                                        {
                                            type = 1;
                                            filesize = (ulong)new FileInfo(path).Length;
                                        }
                                        else if(Directory.Exists(path)) type = 2;
                                        else
                                        {
                                            WriteOutBlockBase(Command.ResultInvalidInput);
                                            break;
                                        }
                                        WriteOutBlockBase(Command.ResultSuccess);
                                        outblock.Write(type);
                                        outblock.Write(filesize);
                                        break;
                                    }
                                case CommandId.GetFileCount:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        if(Directory.Exists(path))
                                        {
                                            if((LastFilesPath != path) || (LastFiles == null))
                                            {
                                                LastFilesPath = path;
                                                LastFiles = Directory.GetFiles(path);
                                            }
                                            uint count = (uint)LastFiles.Length;
                                            WriteOutBlockBase(Command.ResultSuccess);
                                            outblock.Write(count);
                                        }
                                        else WriteOutBlockBase(Command.ResultInvalidInput);
                                        break;
                                    }
                                case CommandId.GetFile:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        var fileidx = inblock.ReadUInt32();
                                        if(Directory.Exists(path))
                                        {
                                            if((LastFilesPath != path) || (LastFiles == null))
                                            {
                                                Program.Warn.LogL("Reloading...");
                                                LastFilesPath = path;
                                                LastFiles = Directory.GetFiles(path);
                                            }

                                            if(fileidx >= LastFiles.Length) WriteOutBlockBase(Command.ResultInvalidInput);
                                            else
                                            {
                                                var filename = Path.GetFileName(LastFiles[fileidx]);
                                                WriteOutBlockBase(Command.ResultSuccess);
                                                outblock.WriteStringEx(filename);
                                            }
                                        }
                                        else WriteOutBlockBase(Command.ResultInvalidInput);
                                        break;
                                    }
                                case CommandId.GetDirectoryCount:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        if(Directory.Exists(path))
                                        {
                                            if((LastDirectoriesPath != path) || (LastDirectories == null))
                                            {
                                                LastDirectoriesPath = path;
                                                LastDirectories = Directory.GetDirectories(path);
                                            }

                                            uint count = (uint)LastDirectories.Length;
                                            WriteOutBlockBase(Command.ResultSuccess);
                                            outblock.Write(count);
                                        }
                                        else WriteOutBlockBase(Command.ResultInvalidInput);
                                        break;
                                    }
                                case CommandId.GetDirectory:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        var diridx = inblock.ReadUInt32();
                                        if(Directory.Exists(path))
                                        {
                                            if((LastDirectoriesPath != path) || (LastDirectories == null))
                                            {
                                                LastDirectoriesPath = path;
                                                LastDirectories = Directory.GetDirectories(path);
                                            }

                                            if (diridx >= LastDirectories.Length) WriteOutBlockBase(Command.ResultInvalidInput);
                                            else
                                            {
                                                var dirname = Path.GetFileName(LastDirectories[diridx]);
                                                WriteOutBlockBase(Command.ResultSuccess);
                                                outblock.WriteStringEx(dirname);
                                            }
                                        }
                                        else WriteOutBlockBase(Command.ResultInvalidInput);
                                        break;
                                    }
                                case CommandId.ReadFile:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        var offset = inblock.ReadUInt64();
                                        var size = inblock.ReadUInt64();
                                        try
                                        {
                                            if(string.IsNullOrEmpty(LastRFile) || (LastRFile != path))
                                            {
                                                if(LastReadFile != null)
                                                {
                                                    LastReadFile.Close();
                                                    LastReadFile = null;
                                                }
                                                LastRFile = path;
                                                LastReadFile = new BinaryReader(new FileStream(LastRFile, FileMode.Open, FileAccess.Read));
                                            }
                                            LastReadFile.BaseStream.Seek((long)offset, SeekOrigin.Begin);
                                            byte[] data = new byte[size];
                                            var rbytes = LastReadFile.Read(data, 0, (int)size);
                                            if(rbytes > 0)
                                            {
                                                WriteOutBlockBase(Command.ResultSuccess);
                                                outblock.Write((ulong)rbytes);
                                                bufs.Add(data);
                                            }
                                        }
                                        catch
                                        {
                                            WriteOutBlockBase(Command.ResultInvalidInput);
                                            break;
                                        }
                                        break;
                                    }
                                case CommandId.WriteFile:
                                    {
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        var size = inblock.ReadUInt64();
                                        var data = USB.ReadBuffer(size);
                                        try
                                        {
                                            if(string.IsNullOrEmpty(LastWFile) || (LastWFile != path))
                                            {
                                                if (LastWriteFile != null)
                                                {
                                                    LastWriteFile.Close();
                                                    LastWriteFile = null;
                                                }
                                                LastWFile = path;
                                                LastWriteFile = new FileStream(path, FileMode.Append, FileAccess.Write);
                                            }
                                            LastWriteFile.Write(data, 0, (int)size);
                                            WriteOutBlockBase(Command.ResultSuccess);
                                        }
                                        catch
                                        {
                                            WriteOutBlockBase(Command.ResultInvalidInput);
                                            break;
                                        }
                                        break;
                                    }
                                case CommandId.Create:
                                    {
                                        var type = inblock.ReadUInt32();
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        switch(type)
                                        {
                                            case 1:
                                                {
                                                    try
                                                    {
                                                        File.Create(path);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            case 2:
                                                {
                                                    try
                                                    {
                                                        Directory.CreateDirectory(path);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            default:
                                                {
                                                    WriteOutBlockBase(Command.ResultInvalidInput);
                                                    break;
                                                }
                                        }
                                        break;
                                    }
                                case CommandId.Delete:
                                    {
                                        var type = inblock.ReadUInt32();
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        switch(type)
                                        {
                                            case 1:
                                                {
                                                    try
                                                    {
                                                        File.Delete(path);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            case 2:
                                                {
                                                    try
                                                    {
                                                        Directory.Delete(path, true);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            default:
                                                {
                                                    WriteOutBlockBase(Command.ResultInvalidInput);
                                                    break;
                                                }
                                        }
                                        break;
                                    }
                                case CommandId.Rename:
                                    {
                                        var type = inblock.ReadUInt32();
                                        var path = inblock.ReadStringEx().NormalizeAsPath();
                                        var pathnew = inblock.ReadStringEx().NormalizeAsPath();
                                        switch(type)
                                        {
                                            case 1:
                                                {
                                                    try
                                                    {
                                                        Microsoft.VisualBasic.FileIO.FileSystem.RenameFile(path, pathnew);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            case 2:
                                                {
                                                    try
                                                    {
                                                        Microsoft.VisualBasic.FileIO.FileSystem.RenameDirectory(path, pathnew);
                                                        WriteOutBlockBase(Command.ResultSuccess);
                                                    }
                                                    catch
                                                    {
                                                        WriteOutBlockBase(Command.ResultInvalidInput);
                                                    }
                                                    break;
                                                }
                                            default:
                                                {
                                                    WriteOutBlockBase(Command.ResultInvalidInput);
                                                    break;
                                                }
                                        }
                                        break;
                                    }
                                case CommandId.GetSpecialPathCount:
                                    {
                                        WriteOutBlockBase(Command.ResultSuccess);
                                        outblock.Write((uint)SpecialPathList.Count);
                                        break;
                                    }
                                case CommandId.GetSpecialPath:
                                    {
                                        var spathidx = inblock.ReadUInt32();
                                        if(spathidx >= SpecialPathList.Count) WriteOutBlockBase(Command.ResultInvalidInput);
                                        else
                                        {
                                            var spath = SpecialPathList.ElementAt((int)spathidx);
                                            var path = Environment.GetFolderPath(spath.Value).NormalizeAsGoldleafPath();
                                            WriteOutBlockBase(Command.ResultSuccess);
                                            outblock.WriteStringEx(spath.Key);
                                            outblock.WriteStringEx(path);
                                        }
                                        break;
                                    }
                                case CommandId.SelectFile:
                                    {
                                        try
                                        {
                                            var openfd = new OpenFileDialog
                                            {
                                                Filter = "Any file (*.*)|*.*",
                                                Title = "Select file for Goldleaf",
                                                Multiselect = false
                                            };
                                            if (openfd.ShowDialog() == DialogResult.OK)
                                            {
                                                WriteOutBlockBase(Command.ResultSuccess);
                                                var path = openfd.FileName.NormalizeAsGoldleafPath();
                                                outblock.WriteStringEx(path);
                                            }
                                            else WriteOutBlockBase(Command.ResultInvalidInput);
                                        }
                                        catch
                                        {
                                            WriteOutBlockBase(Command.ResultInvalidInput);
                                        }
                                        break;
                                    }
                            }
                        }
                    }

                    USB.WritePipe(WritePipeId, outblockbuf, CommandBlockLength, out int writtenbuf, IntPtr.Zero);

                    if(bufs.Any())
                    {
                        foreach(var buf in bufs)
                        {
                            USB.WriteBuffer(buf);
                        }
                    }

                    if(writtenbuf != CommandBlockLength)
                    {
                        Program.Command.LogL($"Command block write size mismatch (expected: 0x{CommandBlockLength:X}, got 0x{writtenbuf:X}!)");
                    }
                }
            }
        }
    }

    public static class Command
    {
        public static readonly uint InputMagic = 0x49434C47;
        public static readonly uint OutputMagic = 0x4F434C47;

        public static readonly uint ResultModule = 356; // Goldleaf result module

        public static uint MakeResult(uint Id)
        {
            return ((ResultModule) & 0x1FF) | ((Id + 100) & 0x1FFF) << 9;
        }

        public static readonly uint ResultSuccess = 0;
        public static readonly uint ResultInvalidInput = MakeResult(1);
    }
}
