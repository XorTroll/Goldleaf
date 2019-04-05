using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using libusbK;

namespace gtree
{
    public enum CommandId
    {
        ConnectionRequest,
        ConnectionResponse,
        NSPName,
        Start,
        NSPData,
        NSPContent,
        NSPTicket,
        Finish, 
    }

    public enum CommandHandleResult
    {
        Success,
        BadInArguments,
    }

    public enum CommandReadResult
    {
        Success,
        InvalidMagic,
        InvalidCommand,
    }

    public enum NewCommandId
    {
        ListSystemDrives,
        GetPathType,
        ListDirectories,
        ListFiles,
        GetFileSize,
        FileRead,
        FileWrite,
        CreateFile,
        CreateDirectory,
        DeleteFile,
        DeleteDirectory,
        RenameFile,
        RenameDirectory,
        GetDriveTotalSpace,
        GetDriveFreeSpace,
        GetNSPContents,
    }

    public static class StringExtras
    {
        public static string NormalizeAsPath(this string Str)
        {
            return Str.Replace('/', '\\');
        }
    }

    public static class UsbKWriteExtras
    {
        public static void WriteBytes(this UsbK USB, byte[] Data)
        {
            USB.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
        }

        public static void Write8(this UsbK USB, byte Data)
        {
            USB.WriteBytes(new byte[] { Data });
        }

        public static void Write32(this UsbK USB, uint Data)
        {
            USB.WriteBytes(BitConverter.GetBytes(Data));
        }

        public static void Write64(this UsbK USB, ulong Data)
        {
            USB.WriteBytes(BitConverter.GetBytes(Data));
        }

        public static void WriteString(this UsbK USB, string Data)
        {
            USB.Write32((uint)Data.Length);
            USB.WriteBytes(Encoding.ASCII.GetBytes(Data));
        }
    }

    public static class UsbKReadExtras
    {
        public static byte[] ReadBytes(this UsbK USB, int Length)
        {
            byte[] b = new byte[Length];
            USB.ReadPipe(0x81, b, Length, out _, IntPtr.Zero);
            return b;
        }

        public static byte Read8(this UsbK USB)
        {
            return USB.ReadBytes(1)[0];
        }

        public static uint Read32(this UsbK USB)
        {
            return BitConverter.ToUInt32(USB.ReadBytes(4), 0);
        }

        public static ulong Read64(this UsbK USB)
        {
            return BitConverter.ToUInt64(USB.ReadBytes(8), 0);
        }

        public static string ReadString(this UsbK USB)
        {
            uint strlen = USB.Read32();
            return Encoding.UTF8.GetString(USB.ReadBytes((int)strlen));
        }

        public static CommandReadResult ReadCommandInput(this UsbK USB, out NewCommandId Type)
        {
            CommandReadResult res = CommandReadResult.Success;
            uint magic = USB.Read32();
            while(magic == 0)
            {
                magic = USB.Read32();
            }
            uint cmdid = USB.Read32();
            Type = (NewCommandId)cmdid;
            return res;
        }

        public static CommandHandleResult HandleNextCommand(this UsbK USB)
        {
            CommandHandleResult res = CommandHandleResult.Success;
            CommandReadResult rres = USB.ReadCommandInput(out NewCommandId cmdid);
            if(rres == CommandReadResult.Success)
            {
                // Console.WriteLine("Received command: " + cmdid.ToString());
                switch(cmdid)
                {
                    case NewCommandId.ListSystemDrives:
                        {
                            var drives = DriveInfo.GetDrives();
                            List<DriveInfo> adrives = new List<DriveInfo>();
                            uint count = (uint)drives.Length;
                            if(drives.Length > 0) foreach(var drive in drives)
                            {
                                if(drive.IsReady) adrives.Add(drive);
                            }
                            USB.Write32((uint)adrives.Count);
                            if(adrives.Count > 0) foreach(var drive in adrives)
                            {
                                string label = drive.VolumeLabel;
                                string prefix = drive.Name.Substring(0, 1);
                                USB.WriteString(label);
                                USB.WriteString(prefix);
                            }
                            break;
                        }
                    case NewCommandId.GetPathType:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            uint ptype = 0;
                            if(File.Exists(path)) ptype = 1;
                            else if(Directory.Exists(path)) ptype = 2;
                            USB.Write32(ptype);
                            break;
                        }
                    case NewCommandId.ListDirectories:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            var ents = Directory.GetDirectories(path);
                            USB.Write32((uint)ents.Length);
                            if(ents.Length > 0) foreach(var ent in ents)
                            {
                                string name = Path.GetFileName(ent);
                                USB.WriteString(name);
                            }
                            break;
                        }
                    case NewCommandId.ListFiles:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            var ents = Directory.GetFiles(path);
                            USB.Write32((uint)ents.Length);
                            if(ents.Length > 0) foreach(var ent in ents)
                            {
                                string name = Path.GetFileName(ent);
                                USB.WriteString(name);
                            }
                            break;
                        }
                    case NewCommandId.GetFileSize:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            ulong size = 0;
                            try
                            {
                                size = (ulong)new FileInfo(path).Length;
                            }
                            catch
                            {
                                size = 0;
                            }
                            USB.Write64(size);
                            break;
                        }
                    case NewCommandId.FileRead:
                        {
                            ulong offset = USB.Read64();
                            ulong size = USB.Read64();
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            ulong rbytes = 0;
                            Console.WriteLine("FileRead - Path: " + path + ", Offset: " + offset + ", Size: " + size);
                            byte[] data = new byte[size];
                            try
                            {
                                var fs = new FileStream(path, FileMode.Open, FileAccess.Read)
                                {
                                    Position = (long)offset
                                };
                                rbytes = (uint)fs.Read(data, 0, (int)size);
                                fs.Close();
                            }
                            catch(Exception ex)
                            {
                                Console.WriteLine("FileRead (" + path + ") - Error reading file: " + ex.Message);
                            }
                            USB.Write64(rbytes);
                            Console.WriteLine("FileRead - Read bytes: " + rbytes);
                            if (rbytes > 0) USB.WriteBytes(data);
                            break;
                        }
                    case NewCommandId.FileWrite:
                        {
                            string spath = USB.ReadString();
                            USB.Read32();
                            uint offset = USB.Read32();
                            uint size = USB.Read32();
                            byte[] data = USB.ReadBytes((int)size);
                            string path = spath.NormalizeAsPath();
                            Console.WriteLine("FileWrite - Path: (" + path + "), Offset: " + offset + ", Size: " + size);
                            try
                            {
                                var fs = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Read);
                                var bw = new BinaryWriter(fs);
                                bw.BaseStream.Position = offset;
                                bw.Write(data, 0, (int)size);
                                bw.Close();
                            }
                            catch
                            {
                                Console.WriteLine("FileWrite (" + path + ") - Error writing file");
                            }
                            break;
                        }
                    case NewCommandId.CreateFile:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            File.Create(path);
                            break;
                        }
                    case NewCommandId.CreateDirectory:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            Directory.CreateDirectory(path);
                            break;
                        }
                    case NewCommandId.DeleteFile:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            File.Delete(path);
                            break;
                        }
                    case NewCommandId.DeleteDirectory:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            Directory.Delete(path, true);
                            break;
                        }
                    case NewCommandId.RenameFile:
                        {
                            string spath = USB.ReadString();
                            string newname = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            string npath = Path.Combine(Path.GetDirectoryName(path), newname);
                            File.Move(path, npath);
                            break;
                        }
                    case NewCommandId.RenameDirectory:
                        {
                            string spath = USB.ReadString();
                            string newname = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            string npath = Path.Combine(Path.GetDirectoryName(path), newname);
                            Directory.Move(path, npath);
                            break;
                        }
                    case NewCommandId.GetDriveTotalSpace:
                        {
                            string sdrive = USB.ReadString();
                            var drives = DriveInfo.GetDrives();
                            ulong space = 0;
                            if(drives.Length > 0) foreach(var drive in drives)
                            {
                                if(drive.Name.StartsWith(sdrive))
                                {
                                    space = (ulong)drive.TotalSize;
                                    break;
                                }
                            }
                            USB.Write64(space);
                            break;
                        }
                    case NewCommandId.GetDriveFreeSpace:
                        {
                            string sdrive = USB.ReadString();
                            var drives = DriveInfo.GetDrives();
                            ulong space = 0;
                            if(drives.Length > 0) foreach(var drive in drives)
                            {
                                if(drive.IsReady && drive.Name.StartsWith(sdrive))
                                {
                                    space = (ulong)drive.TotalFreeSpace;
                                    break;
                                }
                            }
                            USB.Write64(space);
                            break;
                        }
                }
            }
            else res = CommandHandleResult.BadInArguments;
            return res;
        }
    }

    public static class Command
    {
        public static readonly uint GUCIMagic = 0x49435547;
        public static readonly uint GUCOMagic = 0x4f435547;
    }
}
