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
            try
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
                            // Send NSP parts and offsets, install them...
                        }
                    }
                    else Error.Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                }
                else Error.Log("Invalid USB command was received. Are you sure Goldleaf is active?");
            }
            catch
            {
                Error.Log("An error ocurred selecting NSP to be sent.");
            }
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
            Environment.Exit(1);
        }
    }
}