using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using libusbK;
using LibHac;
using LibHac.IO;
using Goldtree.Lib;
using Goldtree.Lib.UsbK;

namespace gtree
{
    public class Program
    {
        [STAThread]
        public static void Main(string[] args)
        {
            CLI cli = new CLI(new Usb());

            cli.Initialize();
            bool success = cli.ConnectUsb();
            if (!success)
                return;

            try
            {
                success = cli.ConnectToGoldleaf();
                if (!success)
                    return;

                CLI.Log.Log("Connection was established with Goldleaf.");
                CLI.Log.Log("Select the NSP to send to Goldleaf on the dialog.");

                string filename;

                if (args.Length == 0)
                {
                    OpenFileDialog fd = new OpenFileDialog()
                    {
                        Title = "Select NSP to send to Goldleaf via USB",
                        Filter = "NSP / Nintendo Submission Package (*.nsp)|*.nsp",
                        Multiselect = false,
                    };

                    if (fd.ShowDialog() != DialogResult.OK)
                    {
                        cli.SendFinish();
                        CLI.Error.Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                        return;
                    }

                    filename = fd.FileName;
                }
                else
                {
                    filename = args[0];
                    CLI.IsArgumentsOnly = true;
                }

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
            if (!CLI.IsArgumentsOnly && Console.ReadKey().Key != ConsoleKey.Enter)
                Main(args);
            else
                cli.SendFinish();
        }
    }
}
