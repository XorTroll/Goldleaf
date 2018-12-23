using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using libusbK;

namespace gtree
{
    public enum LogType
    {
        Information,
        Warning,
        Error,
    }

    public class Program
    {

        public static void Log(string Text, LogType Type = LogType.Error, bool NewLine = true)
        {
            if(NewLine) Console.WriteLine();
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
                    Environment.Exit(1);
                    break;
            }
        }

        [STAThread]
        public static void Main(string[] Args)
        {
            Console.Title = "Goldtree - Goldleaf's USB installation client";
            // Some kind of greeting message...
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
                Log("No USB connection was not found. Make sure you have Goldleaf open before running Goldtree.");
            }
            try
            {
                Command c = new Command(CommandId.ConnectionRequest);
                usb.Write(c);
                Log("Attempting to connect to Goldleaf via USB...", LogType.Information);
                Command rc = usb.Read();
                if((rc.Magic == Command.GLUC) && (rc.CommandId == CommandId.ConnectionResponse))
                {
                    Log("Connection was established with Goldleaf.", LogType.Information, false);
                    Log("Select the NSP to send to Goldleaf on the dialog.", LogType.Information, false);
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
                        Log("NSP name was sent to Goldleaf. Waiting for install instruction...", LogType.Information, false);
                        // ...
                    }
                    else Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                }
            }
            catch
            {
                Log("Error selecting NSP to be sent.");
            }
            Console.ReadKey();
        }
    }
}