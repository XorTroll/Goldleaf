using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using libusbK;

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
                /*if(Program.USB != null)
                {
                    Command c = new Command(CommandId.Finish);
                    Program.USB.Write(c);
                }*/
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

        public static UsbK USB = null;

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
            Console.Write("Copyright (C) 2018 - 2019");
            Console.ForegroundColor = ConsoleColor.DarkYellow;
            Console.Write(" Goldleaf");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write(" project, by");
            Console.ForegroundColor = ConsoleColor.DarkGreen;
            Console.Write(" XorTroll");
            Console.WriteLine();
            Console.ForegroundColor = ConsoleColor.Gray;
            Warn.Log("Make sure to open Goldtree after having launched Goldleaf on your console. Waiting for connection...");
        }

        public static void Main(string[] Args)
        {
            Initialize();
            var pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
            var lst = new LstK(0, ref pat);
            try
            {
                lst.MoveNext(out var dinfo);
                USB = new UsbK(dinfo);
            }
            catch
            {
                // System.Diagnostics.Process.Start(System.Windows.Forms.Application.ExecutablePath);
                return;
            }
            while(USB.HandleNextCommand() == CommandHandleResult.Success);
            /*
        usbnsp:
            // Console.Clear();
            Initialize();
            try
            {
                var pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
                var lst = new LstK(0, ref pat);
                lst.MoveNext(out var dinfo);
                USB = new UsbK(dinfo);
            }
            catch
            {
                Error.Log("No USB connection was found. Make sure you have Goldleaf open before running Goldtree.");
            }
            try
            {
                Command c = new Command(CommandId.ConnectionRequest);
                USB.Write(c);
                Log.Log("Attempting to connect to Goldleaf via USB...");
                Command rc = USB.Read();
                if(rc.MagicOk())
                {
                    if(rc.IsCommandId(CommandId.ConnectionResponse))
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
                            USB.Write(c);
                            USB.Write((uint)nspname.Length);
                            USB.Write(nspname);
                            Log.Log("Selected NSP's name was sent to Goldleaf. Waiting for install approval from Goldleaf...");
                            Command rc2 = USB.Read();
                            if(rc2.MagicOk())
                            {
                                if(rc2.IsCommandId(CommandId.Start))
                                {
                                    Log.Log("Goldleaf is ready for the installation. Preparing everything...");
                                    try
                                    {
                                        FileStream fs = new FileStream(nsp, FileMode.Open);
                                        StreamStorage ist = new StreamStorage(fs, true);
                                        Pfs pnsp = new Pfs(ist);
                                        ist.Dispose();
                                        fs.Close();
                                        fs = new FileStream(nsp, FileMode.Open);
                                        uint filecount = (uint)pnsp.Files.Length;
                                        c = new Command(CommandId.NSPData);
                                        USB.Write(c);
                                        USB.Write(filecount);
                                        int tikidx = -1;
                                        int certidx = -1;
                                        int tmpidx = 0;
                                        foreach(var file in pnsp.Files)
                                        {
                                            ulong offset = (ulong)pnsp.HeaderSize + (ulong)file.Offset;
                                            ulong size = (ulong)file.Size;
                                            uint len = (uint)file.Name.Length;
                                            USB.Write(len);
                                            USB.Write(file.Name);
                                            USB.Write(offset);
                                            USB.Write(size);
                                            if(Path.GetExtension(file.Name).Replace(".", "").ToLower() == "tik") tikidx = tmpidx;
                                            else if(Path.GetExtension(file.Name).Replace(".", "").ToLower() == "cert") certidx = tmpidx;
                                            tmpidx++;
                                        }
                                        BinaryReader br = new BinaryReader(fs);
                                        while(true)
                                        {
                                            Command ccmd = USB.Read();
                                            if(ccmd.MagicOk())
                                            {
                                                if(ccmd.IsCommandId(CommandId.NSPContent))
                                                {
                                                    USB.Read(out uint idx);
                                                    Log.Log("Sending content \'" + pnsp.Files[idx].Name + "\'... (" + (idx + 1) + " of " + pnsp.Files.Length + ")");
                                                    PfsFileEntry ent = pnsp.Files[idx];
                                                    long rsize = 0x100000;
                                                    long coffset = pnsp.HeaderSize + ent.Offset;
                                                    long toread = ent.Size;
                                                    long tmpread = 1;
                                                    byte[] bufb;
                                                    while((tmpread > 0) && (toread > 0) && (coffset < (coffset + ent.Size)))
                                                    {
                                                        if(rsize >= ent.Size) rsize = ent.Size;
                                                        int tor = (int)Math.Min(rsize, toread);
                                                        br.BaseStream.Position = coffset;
                                                        bufb = br.ReadBytes(tor);
                                                        tmpread = bufb.Length;
                                                        USB.Write(bufb);
                                                        coffset += tmpread;
                                                        toread -= tmpread;
                                                    }
                                                    Log.Log("Content was sent to Goldleaf.");
                                                }
                                                else if(ccmd.IsCommandId(CommandId.NSPTicket))
                                                {
                                                    Log.Log("Sending ticket file...");
                                                    PfsFileEntry tik = pnsp.Files[tikidx];
                                                    br.BaseStream.Seek(pnsp.HeaderSize + tik.Offset, SeekOrigin.Begin);
                                                    byte[] file = br.ReadBytes((int)tik.Size);
                                                    USB.Write(file);
                                                    Log.Log("Ticket was sent to Goldleaf.");
                                                }
                                                else if(ccmd.IsCommandId(CommandId.Finish)) break;
                                                else
                                                {
                                                    USB = null;
                                                    Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                                                }
                                            }
                                            else
                                            {
                                                USB = null;
                                                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                                            }
                                        }
                                    }
                                    catch
                                    {
                                        Error.Log("An error ocurred opening the selected NSP. Are you sure it's a valid NSP?");
                                    }
                                }
                                else if(rc2.IsCommandId(CommandId.Finish))
                                {
                                    USB = null;
                                    Error.Log("Goldleaf has canceled the installation.");
                                }
                                else
                                {
                                    USB = null;
                                    Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                                }
                            }
                            else
                            {
                                USB = null;
                                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                            }
                        }
                        else Error.Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                    }
                    else if(rc.IsCommandId(CommandId.Finish))
                    {
                        USB = null;
                        Error.Log("Goldleaf has canceled the installation.");
                    }
                    else
                    {
                        USB = null;
                        Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                    }
                }
                else
                {
                    USB = null;
                    Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                }
            }
            catch
            {
                Error.Log("An error ocurred selecting the NSP to be sent.");
            }
            Log.Log("The installation has finished. Press ENTER to close Goldtree, or any other key to start another USB installation.", true);
            ConsoleKeyInfo ki = Console.ReadKey();
            if(ki.Key == ConsoleKey.Enter)
            {
                if(USB != null)
                {
                    Command cmd = new Command(CommandId.Finish);
                    USB.Write(cmd);
                }
                Environment.Exit(0);
            }
            else goto usbnsp;
            */
        }
    }
}
