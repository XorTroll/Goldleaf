using System;
using System.IO;
using System.Text;
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
            if(Close)
            {
                Console.WriteLine();
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
                Environment.Exit(0);
            }
        }

        public void LLog(string Text)
        {
            Console.WriteLine();
            Log(Text);
        }

        public void LogL(string Text)
        {
            Log(Text);
            Console.WriteLine();
        }
        public void LLogL(string Text)
        {

            Console.WriteLine();
            Log(Text);
            Console.WriteLine();
        }
    }

    public class Program
    {
        public static readonly string Name = "Goldtree";
        public static readonly string Description = "Goldleaf's USB PC client";

        public static readonly LogMode Log = new LogMode("Log", ConsoleColor.Cyan);
        public static readonly LogMode Warn = new LogMode("Warn", ConsoleColor.Yellow);
        public static readonly LogMode Command = new LogMode("Command", ConsoleColor.Green);
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
        }

        public static void Main(string[] Args)
        {
            Initialize();
            Warn.LLogL("Make sure to open Goldtree after having opened Goldleaf on your console. Looking for connection...");
            try
            {
                var pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
                var lst = new LstK(0, ref pat);
                lst.MoveNext(out var dinfo);
                USB = new UsbK(dinfo);
                Log.LogL("Connection was established.");
            }
            catch
            {
                Error.LLogL("Unable to find USB communication. Are you sure Goldleaf is open and plugged in?");
            }
            Console.WriteLine();
            while(USB.HandleNextCommand());
            Error.LLogL("An error ocurred handling Goldleaf's latest command request.");
        }
    }
}
