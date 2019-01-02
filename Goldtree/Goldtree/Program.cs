using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using libusbK;
using LibHac;
using LibHac.IO;
using Goldtree.Lib;

namespace gtree
{
    public class Program
    {
        public static void Main(string[] Args)
        {
            CLI.Instance.Initialize();

            UsbK usb = CLI.Instance.GetUsb();
            try
            {
                bool success = CLI.Instance.ConnectToGoldleaf(usb);
                if (!success)
                    return;

                CLI.Log.Log("Connection was established with Goldleaf.");
                CLI.Log.Log("Select the NSP to send to Goldleaf on the dialog.");

                OpenFileDialog fd = new OpenFileDialog()
                {
                    Title = "Select NSP to send to Goldleaf via USB",
                    Filter = "NSP / Nintendo Submission Package (*.nsp)|*.nsp",
                    Multiselect = false,
                };

                if (fd.ShowDialog() != DialogResult.OK)
                {
                    CLI.Instance.SendFinish(usb);
                    CLI.Error.Log("The dialog was closed without selecting a NSP, or another error ocurred. Reopen Goldleaf and Goldtree and try again.");
                    return;
                }

                string filename = fd.FileName;

                success = CLI.Instance.SendFileName(usb, filename);
                if (!success)
                    return;

                success = CLI.Instance.SendFileContent(usb, filename);
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
                CLI.Instance.SendFinish(usb);
        }
    }
}
