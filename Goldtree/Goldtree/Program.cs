using System;
using System.Text;
using System.Windows.Forms;
using libusbK;

namespace gleaf.tree
{
    public enum LogType
    {
        Information,
        Warning,
        Error,
    }

    public class Program
    {

        public static void Log(string Text, LogType Type = LogType.Error)
        {
            Console.WriteLine();
            switch(Type)
            {
                case LogType.Information:
                    Console.WriteLine("[tree:Information] " + Text);
                    break;
                case LogType.Warning:
                    Console.WriteLine("[tree:Warning] " + Text);
                    break;
                case LogType.Error:
                    Console.WriteLine("[tree:Error] " + Text);
                    Console.WriteLine("Press any key to exit.");
                    Console.ReadKey();
                    break;
            }
        }

        [STAThread]
        public static void Main(string[] Args)
        {
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
                Log("No USB connection was not found. Make sure you have Goldleaf open.");
            }
            try
            {
                Command c = new Command(0);
                Commands.SendCommand(usb, c);
                Log("Attempting to connect to Goldleaf via USB...", LogType.Information);
                Command rc = Commands.ReceiveCommand(usb);
                if(rc.Magic == Commands.GLUC) Log("Connection established (Id = " + rc.CommandId + ")");
                while (true) ;
            }
            catch
            {
                Log("Error selecting NSP to be sent.");
            }
            Console.ReadKey();
        }
    }
}
