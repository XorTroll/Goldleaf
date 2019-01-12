using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using libusbK;
using LibHac;
using LibHac.IO;

namespace gtree
{
    public class LogMode
    {
        public string Type { get; set; }
        public ConsoleColor Color { get; set; }

        public LogMode(string Type, ConsoleColor Color)
        {
            this.Type = Type;
            this.Color = Color;
        }

        public void Log(string Text)
        {
            ConsoleColor prev = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("[");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write("gtree");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.Write(":");
            Console.ForegroundColor = Color;
            Console.Write(Type);
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("]");
            Console.ForegroundColor = prev;
            Console.Write(" ");
            Console.Write(Text);
        }
    }

    public class GoldTree
    {
        public static readonly string Name = "Goldtree";
        public static readonly string Description = "Goldleaf's USB installation client";

        public readonly LogMode Log = new LogMode("Log", ConsoleColor.Cyan);
        public readonly LogMode Warn = new LogMode("Warn", ConsoleColor.Yellow);
        public readonly LogMode Error = new LogMode("Error", ConsoleColor.Red);

        private UsbK USB;

        public GoldTree()
        {
            Console.Clear();
            this.PrintNotice();
            this.InitializeUSBConnection();
            this.StartInstall();

            Log.Log(Messages.InstallationFinished);
            Console.ReadKey();
            Environment.Exit(0);
        }

        private void PrintNotice()
        {
            Console.Title = Name +  " - " + Description;
            Console.WriteLine();
            Console.Write("    ");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write(Name);
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.Write(" - ");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write(Description);
            Console.WriteLine();
            Console.Write("    ");
            Console.Write("Copyright (C) 2018 - 2019");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write(" Goldleaf");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write(" project, by");
            Console.ForegroundColor = ConsoleColor.DarkGreen;
            Console.Write(" XorTroll");
            Console.WriteLine();
            Console.ForegroundColor = ConsoleColor.Gray;
            Warn.Log(Messages.EnsureConnection);
        }

        private void InitializeUSBConnection()
        {
            
            Log.Log(Messages.ConnectingToUSB);
            try
            {
                var pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
                var lst = new LstK(0, ref pat);
                lst.MoveNext(out var dinfo);
                this.USB = new UsbK(dinfo);
            }
            catch (libusbK.DriverLoadException)
            {
                this.QuitWithError(Messages.USBDriverLoadError);
            }
            catch (libusbK.DriverInitException)
            {
                this.QuitWithError(Messages.USBDriverInitError);
            }
            catch (Exception)
            {
                this.QuitWithError(Messages.USBConnectionError);
            }
        }

        private string GetNSPPath()
        {
            Log.Log(Messages.NSPSelect);
            OpenFileDialog fd = new OpenFileDialog()
            {
                Title = Messages.NSPFileDialogTitle,
                Filter = Messages.NSPExtension,
                Multiselect = false,
            };
            if (fd.ShowDialog() != DialogResult.OK)
            {
                this.QuitWithError(Messages.NSPSelectError);
            }
            return fd.FileName;
        }

        private void StartInstall()
        {
            try
            {
                Command c = new Command(CommandId.ConnectionRequest);
                USB.Write(c);
                Log.Log(Messages.ConnectingToGoldleaf);
                Command rc = USB.Read();
                if (!rc.MagicOk())
                {
                    this.QuitWithError(Messages.USBInvalidCommandReceived);
                }
                if (rc.IsCommandId(CommandId.Finish))
                {
                    this.QuitWithError(Messages.GoldleafCancelledInstall);
                }
                if (!rc.IsCommandId(CommandId.ConnectionResponse))
                {
                    this.QuitWithError(Messages.USBInvalidCommandReceived);
                }

                Log.Log(Messages.GoldleafConnectionEstablished);
                string nsp = this.GetNSPPath();
                string nspname = Path.GetFileName(nsp);
                c = new Command(CommandId.NSPName);
                USB.Write(c);
                USB.Write((uint)nspname.Length);
                USB.Write(nspname);
                Log.Log(Messages.GoldleafNSPApproval);

                Command rc2 = USB.Read();
                if (!rc2.MagicOk())
                {
                    this.QuitWithError(Messages.USBInvalidCommandReceived);
                }
                if (rc2.IsCommandId(CommandId.Finish))
                {
                    this.QuitWithError(Messages.GoldleafCancelledInstall);
                }
                if (!rc2.IsCommandId(CommandId.Start))
                {
                    this.QuitWithError(Messages.USBInvalidCommandReceived);
                }

                this.SendNSPOverUSB(nsp);    
            }
            catch
            {
                this.QuitWithError(Messages.NSPGeneralError);
            }
        }

        private void SendNSPOverUSB(string nsp)
        {
            Log.Log(Messages.GoldleafStartingInstall);
            try
            {
                FileStream fs = new FileStream(nsp, FileMode.Open);
                StreamStorage ist = new StreamStorage(fs, true);
                Pfs pnsp = new Pfs(ist);
                ist.Dispose();
                fs.Close();
                fs = new FileStream(nsp, FileMode.Open);
                uint filecount = (uint)pnsp.Files.Length;
                Command c = new Command(CommandId.NSPData);
                USB.Write(c);
                USB.Write(filecount);
                int tikidx = -1;
                int certidx = -1;
                int tmpidx = 0;
                foreach (var file in pnsp.Files)
                {
                    ulong offset = (ulong)pnsp.HeaderSize + (ulong)file.Offset;
                    ulong size = (ulong)file.Size;
                    uint len = (uint)file.Name.Length;
                    USB.Write(len);
                    USB.Write(file.Name);
                    USB.Write(offset);
                    USB.Write(size);
                    if (Path.GetExtension(file.Name).Replace(".", "").ToLower() == "tik")
                    {
                        tikidx = tmpidx;
                    }
                    else if (Path.GetExtension(file.Name).Replace(".", "").ToLower() == "cert")
                    {
                        certidx = tmpidx;
                    }
                    tmpidx++;
                }
                BinaryReader br = new BinaryReader(fs);
                while (true)
                {
                    Command ccmd = USB.Read();
                    if (!ccmd.MagicOk())
                    {
                        this.QuitWithError(Messages.USBInvalidCommandReceived);
                    }
                    if (ccmd.IsCommandId(CommandId.NSPContent))
                    {
                        USB.Read(out uint idx);
                        Log.Log(String.Format(
                            Messages.GoldleafSendingContent,
                            pnsp.Files[idx].Name,
                            idx + 1,
                            pnsp.Files.Length
                        ));
                        PfsFileEntry ent = pnsp.Files[idx];
                        long rsize = 1048576;
                        long coffset = pnsp.HeaderSize + ent.Offset;
                        long toread = ent.Size;
                        long tmpread = 1;
                        byte[] bufb;
                        while ((tmpread > 0) && (toread > 0) && (coffset < (coffset + ent.Size)))
                        {
                            if (rsize >= ent.Size) rsize = ent.Size;
                            int tor = (int)Math.Min(rsize, toread);
                            bufb = new byte[tor];
                            br.BaseStream.Position = coffset;
                            tmpread = br.Read(bufb, 0, bufb.Length);
                            USB.Write(bufb);
                            coffset += tmpread;
                            toread -= tmpread;
                        }
                        Log.Log(Messages.GoldleafContentSent);
                    }
                    else if (ccmd.IsCommandId(CommandId.NSPTicket))
                    {
                        Log.Log(Messages.GoldleafSendingTicket);
                        PfsFileEntry tik = pnsp.Files[tikidx];
                        br.BaseStream.Seek(pnsp.HeaderSize + tik.Offset, SeekOrigin.Begin);
                        byte[] file = br.ReadBytes((int)tik.Size);
                        USB.Write(file);
                        Log.Log(Messages.GoldleafTicketSent);
                    }
                    else if (ccmd.IsCommandId(CommandId.NSPCert))
                    {
                        Log.Log(Messages.GoldleafSendingCertificate);
                        PfsFileEntry cert = pnsp.Files[certidx];
                        br.BaseStream.Seek(pnsp.HeaderSize + cert.Offset, SeekOrigin.Begin);
                        byte[] file = br.ReadBytes((int)cert.Size);
                        USB.Write(file);
                        Log.Log(Messages.GoldleafCertificateSent);
                    }
                    else if (ccmd.IsCommandId(CommandId.Finish)) break;
                    else
                    {
                        this.QuitWithError(Messages.USBInvalidCommandReceived);
                    }
                }
            }
            catch
            {
                this.QuitWithError(Messages.InvalidNSPError);
            }
        }

        private void QuitWithError(string message)
        {
            this.CloseUSBConnection();
            Error.Log(message);
            Console.WriteLine(Messages.PressAnyKey);
            Console.ReadKey();
            Environment.Exit(1);
        }

        private void CloseUSBConnection()
        {
            if (this.USB != null)
            {
                Command cmd = new Command(CommandId.Finish);
                USB.Write(cmd);
            }
        }

        [STAThread]
        public static void Main(string[] Args)
        {
            new GoldTree();
        }
    }
}
