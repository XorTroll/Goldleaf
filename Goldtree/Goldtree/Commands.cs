using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using libusbK;

namespace gtree
{
    public enum CommandReadResult
    {
        Success,
        InvalidMagic,
        InvalidCommandId,
    }

    public enum CommandId
    {
        ListSystemDrives,
        GetEnvironmentPaths,
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

    public static class UsbKWriteExtras
    {
        public static void WriteBytes(this UsbK USB, byte[] Data)
        {
            USB.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
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
            return Encoding.ASCII.GetString(USB.ReadBytes((int)strlen));
        }

        public static CommandReadResult ReadCommandInput(this UsbK USB, out CommandId Type)
        {
            CommandReadResult res = CommandReadResult.Success;
            uint magic = 0;
            do
            {
                magic = USB.Read32();
            } while(magic != Command.GLUC);
            uint cmdid = USB.Read32();
            Type = (CommandId)cmdid;
            if(magic != Command.GLUC) res = CommandReadResult.InvalidMagic;
            if(cmdid >= (uint)CommandId.Max) res = CommandReadResult.InvalidCommandId;
            return res;
        }

        public static bool HandleNextCommand(this UsbK USB)
        {
            bool ok = true;
            CommandReadResult rres = USB.ReadCommandInput(out CommandId cmdid);
            if(rres == CommandReadResult.Success)
            {
                Program.Command.LogL("Received request (Command: " + cmdid + ")");
                switch(cmdid)
                {
                    case CommandId.ListSystemDrives:
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
                    case CommandId.GetEnvironmentPaths:
                        {
                            var envpaths = new Dictionary<string, Environment.SpecialFolder>()
                            {
                                { "Desktop", Environment.SpecialFolder.Desktop },
                                { "Documents", Environment.SpecialFolder.MyDocuments },
                            };
                            USB.Write32((uint)envpaths.Count);
                            foreach(var path in envpaths)
                            {
                                string pth = Environment.GetFolderPath(path.Value);
                                string spth = pth.NormalizeAsGoldleafPath();
                                USB.WriteString(path.Key);
                                USB.WriteString(spth);
                            }
                            break;
                        }
                    case CommandId.GetPathType:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            uint ptype = 0;
                            if(File.Exists(path)) ptype = 1;
                            else if(Directory.Exists(path)) ptype = 2;
                            USB.Write32(ptype);
                            break;
                        }
                    case CommandId.ListDirectories:
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
                    case CommandId.ListFiles:
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
                    case CommandId.GetFileSize:
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
                    case CommandId.FileRead:
                        {
                            ulong offset = USB.Read64();
                            ulong size = USB.Read64();
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            ulong rbytes = 0;
                            byte[] data = new byte[size];
                            try
                            {
                                var fs = new FileStream(path, FileMode.Open, FileAccess.Read)
                                {
                                    Position = (long)offset,
                                };
                                rbytes = (uint)fs.Read(data, 0, (int)size);
                                fs.Close();
                            }
                            catch
                            {
                                
                            }
                            finally
                            {
                                USB.Write64(rbytes);
                                if(rbytes > 0) USB.WriteBytes(data);
                            }
                            break;
                        }
                    case CommandId.FileWrite:
                        {
                            ulong offset = USB.Read64();
                            ulong size = USB.Read64();
                            string spath = USB.ReadString();
                            byte[] data = USB.ReadBytes((int)size);
                            string path = spath.NormalizeAsPath();
                            try
                            {
                                var fs = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write)
                                {
                                    Position = (long)offset,
                                };
                                fs.Write(data, 0, (int)size);
                                fs.Close();
                            }
                            catch
                            {
                            }
                            break;
                        }
                    case CommandId.CreateFile:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            File.Create(path);
                            break;
                        }
                    case CommandId.CreateDirectory:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            Directory.CreateDirectory(path);
                            break;
                        }
                    case CommandId.DeleteFile:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            File.Delete(path);
                            break;
                        }
                    case CommandId.DeleteDirectory:
                        {
                            string spath = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            Directory.Delete(path, true);
                            break;
                        }
                    case CommandId.RenameFile:
                        {
                            string spath = USB.ReadString();
                            string newname = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            string npath = Path.Combine(Path.GetDirectoryName(path), newname);
                            File.Move(path, npath);
                            break;
                        }
                    case CommandId.RenameDirectory:
                        {
                            string spath = USB.ReadString();
                            string newname = USB.ReadString();
                            string path = spath.NormalizeAsPath();
                            string npath = Path.Combine(Path.GetDirectoryName(path), newname);
                            Directory.Move(path, npath);
                            break;
                        }
                    case CommandId.GetDriveTotalSpace:
                        {
                            string sdrive = USB.ReadString();
                            var drives = DriveInfo.GetDrives();
                            ulong space = 0;
                            if(drives.Length > 0) foreach(var drive in drives)
                            {
                                if(drive.IsReady && (drive.Name == sdrive))
                                {
                                    space = (ulong)drive.TotalSize;
                                    break;
                                }
                            }
                            USB.Write64(space);
                            break;
                        }
                    case CommandId.GetDriveFreeSpace:
                        {
                            string sdrive = USB.ReadString();
                            var drives = DriveInfo.GetDrives();
                            ulong space = 0;
                            if(drives.Length > 0) foreach(var drive in drives)
                            {
                                if(drive.IsReady && (drive.Name == sdrive))
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
            else ok = false;
            return ok;
        }
    }

    public static class Command
    {
        public static readonly uint GLUC = 0x43554c47;
    }
}
