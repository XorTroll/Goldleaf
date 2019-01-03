using System;
using System.IO;
using System.Text;
using libusbK;
using LibHac;
using LibHac.IO;
using Goldtree.Lib;
using Goldtree.Lib.UsbK;

namespace gtree
{
    public class Program
    {
        public static void Main(string[] Args)
        {
            CLI cli = new CLI(new Usb());
            cli.Initialize();
            cli.ConnectUsb();
            try
            {
                bool success = cli.ConnectToGoldleaf();
                if (!success)
                    return;

                CLI.Log.Log("Connection was established with Goldleaf.");
                CLI.Log.Log("Select the NSP to send to Goldleaf on the dialog.");
                Console.WriteLine("Select NSP to send to Goldleaf via USB");
                Console.Write("> ");

                string filename = Console.ReadLine();
                success = cli.SendFile(filename);
                if (!success)
                    return;
            }
            catch
            {
                CLI.Error.Log("An error ocurred selecting the NSP to be sent.");
                return;
            }

            CLI.Log.Log("The installation has finished. Press ENTER to close Goldtree, or any other key to start another USB installation.", true);
            ConsoleKeyInfo ki = Console.ReadKey();
            if (ki.Key != ConsoleKey.Enter)
                Main(Args);
            else
                cli.SendFinish();
        }
    }
}
