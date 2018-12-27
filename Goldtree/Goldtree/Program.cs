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
        public bool Close { get; set; }

        public LogMode(string Type, ConsoleColor Color, bool Close = false)
        {
            this.Type = Type;
            this.Color = Color;
            this.Close = Close;
        }

        public void Log(string Text, bool NewLine = true)
        {
            ConsoleColor prev = Console.ForegroundColor;
            if(NewLine) Console.WriteLine();
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
            if(NewLine) Console.WriteLine();
            if(Close)
            {
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
                Environment.Exit(1);
            }
        }
    }

    public class Program
    {
        public static readonly string Name = "Goldtree";
        public static readonly string Description = "Goldleaf's USB installation client";

        public static readonly LogMode Log = new LogMode("Log", ConsoleColor.Cyan);
        public static readonly LogMode Warn = new LogMode("Warn", ConsoleColor.Yellow);
        public static readonly LogMode Error = new LogMode("Error", ConsoleColor.Red, true);

        public static void Initialize()
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
            Console.Write("Copyright (C) 2018 ");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.Write("-");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write(" Goldleaf");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write(" project, by");
            Console.ForegroundColor = ConsoleColor.DarkGreen;
            Console.Write(" XorTroll");
            Console.WriteLine();
            Console.ForegroundColor = ConsoleColor.Gray;
            Warn.Log("Make sure to open Goldtree AFTER having opened USB installation mode in Goldleaf!");
        }

        [STAThread]
        public static void Main(string[] Args)
        {
            Initialize();
            UsbK usb = null;
            try
            {
                var pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
                var lst = new LstK(0, ref pat);
                lst.MoveNext(out var dinfo);
                usb = new UsbK(dinfo);
            }
            catch
            {
                Error.Log("No USB connection was not found. Make sure you have Goldleaf open before running Goldtree.");
            }
            // try
            {
                Command c = new Command(CommandId.ConnectionRequest);
                usb.Write(c);
                Log.Log("Attempting to connect to Goldleaf via USB...");
                Command rc = usb.Read();
                if(rc.MagicOk() && rc.IsCommandId(CommandId.ConnectionResponse))
                {
                    Log.Log("Connection was established with Goldleaf.");
                    Log.Log("Select the NSP to send to Goldleaf on the dialog.");
                    OpenFileDialog fd = new OpenFileDialog()
                    {
                        Title = "Select NSP to send to Goldleaf via USB",
                        Filter = "NSP / Nintendo Submission Package (*.nsp)|*.nsp",
                        Multiselect = false,
                    };
                    if(fd.ShowDialog() == DialogResult.OK)
                    {
                        string nsp = fd.FileName;
                        string nspname = Path.GetFileName(nsp);
                        c = new Command(CommandId.NSPName);
                        usb.Write(c);
                        usb.Write((uint)nspname.Length);
                        usb.Write(nspname);
                        Log.Log("Selected NSP's name was sent to Goldleaf. Waiting for install approval from Goldleaf...");
                        Command rc2 = usb.Read();
                        if(rc2.MagicOk() && rc2.IsCommandId(CommandId.Start))
                        {
                            Log.Log("Goldleaf is ready for the installation. Preparing everything...");
                            // try
                            {
                                FileStream fs = new FileStream(nsp, FileMode.Open);
                                StreamStorage ist = new StreamStorage(fs, true);
                                Pfs pnsp = new Pfs(ist);
                                ist.Dispose();
                                fs.Close();
                                fs = new FileStream(nsp, FileMode.Open);
                                uint filecount = (uint)pnsp.Files.Length;
                                c = new Command(CommandId.NSPData);
                                usb.Write(c);
                                usb.Write(filecount);
                                int tikidx = -1;
                                int certidx = -1;
                                int tmpidx = 0;
                                foreach(var file in pnsp.Files)
                                {
                                    ulong offset = (ulong)pnsp.HeaderSize + (ulong)file.Offset;
                                    ulong size = (ulong)file.Size;
                                    uint len = (uint)file.Name.Length;
                                    usb.Write(len);
                                    usb.Write(file.Name);
                                    usb.Write(offset);
                                    usb.Write(size);
                                    if(Path.GetExtension(file.Name).Replace(".", "").ToLower() == "tik") tikidx = tmpidx;
                                    else if(Path.GetExtension(file.Name).Replace(".", "").ToLower() == "cert") certidx = tmpidx;
                                    tmpidx++;
                                }
                                BinaryReader br = new BinaryReader(fs);
                                while(true)
                                {
                                    Command ccmd = usb.Read();
                                    if(ccmd.MagicOk())
                                    {
                                        if(ccmd.IsCommandId(CommandId.NSPContent))
                                        {
                                            usb.Read(out uint idx);
                                            Console.WriteLine(idx + " of " + pnsp.Files.Length);
                                            PfsFileEntry ent = pnsp.Files[idx];
                                            long rsize = 1048576;
                                            long coffset = pnsp.HeaderSize + ent.Offset;
                                            long toread = ent.Size;
                                            long tmpread = 1;
                                            byte[] bufb = new byte[rsize];
                                            while((tmpread > 0) && (toread > 0))
                                            {
                                                Console.WriteLine("Offset: " + coffset);
                                                int tor = (int)Math.Min(rsize, toread);
                                                bufb = new byte[tor];
                                                br.BaseStream.Position = coffset;
                                                tmpread = br.Read(bufb, 0, bufb.Length);
                                                Console.WriteLine("Read size: " + tmpread);
                                                usb.Write(bufb);
                                                coffset += tmpread;
                                                toread -= tmpread;
                                            }
                                            Console.WriteLine("END");
                                            while (true) ;
                                        }
                                        else if(ccmd.IsCommandId(CommandId.NSPTicket))
                                        {
                                            PfsFileEntry tik = pnsp.Files[tikidx];
                                            br.BaseStream.Seek(tik.Offset, SeekOrigin.Begin);
                                            byte[] file = br.ReadBytes((int)tik.Size);
                                            usb.Write(file);
                                        }
                                        else if(ccmd.IsCommandId(CommandId.NSPCert))
                                        {
                                            PfsFileEntry cert = pnsp.Files[certidx];
                                            br.BaseStream.Seek(cert.Offset, SeekOrigin.Begin);
                                            byte[] file = br.ReadBytes((int)cert.Size);
                                            usb.Write(file);
                                        }
                                    }
                                    else break;
                                }
                            }
                            // catch(Exception ex)
                            {
                                Error.Log("An error ocurred opening the selected NSP. Is it sa valid NSP? Is it in an unauthorized path? Error: ");// + ex.StackTrace);
                            }
                        }
                    }
                    else Error.Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                }
                else Error.Log("Invalid USB command was received. Are you sure Goldleaf is active?");
            }
            // catch
            {
                Error.Log("An error ocurred selecting NSP to be sent.");
            }
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
            Environment.Exit(1);
        }
    }
}