using Goldtree.Lib.Commands;
using LibHac;
using LibHac.IO;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Goldtree.Lib
{
    public class CLI
    {
        public const string Name = "Goldtree";
        public const string Description = "Goldleaf's USB installation client";

        public static readonly Logger Log = new Logger("Log", ConsoleColor.Cyan);
        public static readonly Logger Warn = new Logger("Warn", ConsoleColor.Yellow);
        public static readonly Logger Error = new Logger("Error", ConsoleColor.Red, true);

        private IUsb usb;

        public CLI(IUsb usb)
        {
            this.usb = usb;
        }

        public void Initialize()
        {
            Console.Clear();
            SetTitle();
            PrintBanner();
        }

        public void SetTitle()
        {
            Console.Title = Name + " - " + Description;
        }

        public void PrintBanner()
        {
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
            Warn.Log("Make sure to open Goldtree after having launched Goldleaf on your console. Waiting for connection...");
        }

        public bool ConnectUsb()
        {
            try
            {
                usb.Connect(@"USB\VID_057E&PID_3000");
                return true;
            }
            catch
            {
                Error.Log("No USB connection was found. Make sure you have Goldleaf open before running Goldtree.");
                return false;
            }
        }

        public bool ConnectToGoldleaf()
        {
            Command c = new CommandConnectionRequest();
            c.Send(usb);

            Log.Log("Attempting to connect to Goldleaf via USB...");
            Command rc = Command.Receive(usb);
            if (!rc.MagicOk())
            {
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                return false;
            }

            switch(rc)
            {
                case CommandConnectionResponse _:
                    return true;
                case CommandFinish _:
                    Error.Log("Goldleaf has canceled the installation.");
                    return false;
                default:
                    Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                    return false;

            }
        }

        public bool SendFile(string file)
        {
            if (!file.ToLower().EndsWith(".nsp"))
            {
                SendFinish();
                Error.Log("The selected file has to end with '.nsp'.");
                return false;
            }

            if (!File.Exists(file))
            {
                SendFinish();
                Error.Log("The selected file does not exist.");
                return false;
            }

            bool success = SendFileName(file);
            if (!success)
                return false;

            success = SendFileContent(file);
            if (!success)
                return false;

            return true;
        }

        public bool SendFileName(string file)
        {
            string nspname = Path.GetFileName(file);

            Command c = new CommandNSPName(nspname);
            c.Send(usb);

            Log.Log("Selected NSP's name was sent to Goldleaf. Waiting for install approval from Goldleaf...");
            Command rc = Command.Receive(usb);
            if (!rc.MagicOk())
            {
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                return false;
            }

            switch(rc)
            {
                case CommandStart _:
                    Log.Log("Goldleaf has received the name of the file.");
                    return true;
                case CommandFinish _:
                    Error.Log("Goldleaf has canceled the installation.");
                    return false;
                default:
                    Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                    return false;
            }
        }

        public bool SendFileContent(string fileName)
        {
            Log.Log("Goldleaf is ready for the installation. Preparing everything...");
            try
            {
                Pfs pnsp = ReadPnsp(fileName);
                var (tikIdx, certIdx) = SendNSPData(pnsp);

                using (FileStream fs = new FileStream(fileName, FileMode.Open))
                using (BinaryReader br = new BinaryReader(fs))
                {
                    Command ccmd = Command.Receive(usb);
                    while(!ccmd.MagicOk() || !ccmd.IsCommandId(CommandId.Finish))
                    {
                        if (!ccmd.MagicOk())
                        {
                            Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                            return false;
                        }

                        switch (ccmd)
                        {
                            case CommandNSPContent data:
                                Log.Log("Sending content \'" + pnsp.Files[data.Index].Name + "\'... (" + (data.Index + 1) + " of " + pnsp.Files.Length + ")");
                                PfsFileEntry ent = pnsp.Files[data.Index];
                                long rsize = 1048576;
                                long coffset = pnsp.HeaderSize + ent.Offset;
                                long toread = ent.Size;
                                long tmpread = 1;
                                byte[] bufb;
                                while ((tmpread > 0) && (toread > 0) && (coffset < (coffset + ent.Size)))
                                {
                                    if (rsize >= ent.Size) rsize = ent.Size;
                                    int tor = (int)Math.Min(rsize, toread);
                                    br.BaseStream.Position = coffset;
                                    bufb = br.ReadBytes(tor);
                                    usb.Write(bufb);
                                    coffset += tor;
                                    toread -= tor;
                                }
                                Log.Log("Content was sent to Goldleaf.");
                                break;
                            case CommandNSPTicket _:
                                Log.Log("Sending ticket file...");
                                PfsFileEntry tik = pnsp.Files[tikIdx];
                                br.BaseStream.Seek(pnsp.HeaderSize + tik.Offset, SeekOrigin.Begin);
                                byte[] file = br.ReadBytes((int)tik.Size);
                                usb.Write(file);
                                Log.Log("Ticket was sent to Goldleaf.");
                                break;
                            case CommandNSPCert _:
                                Log.Log("Sending certificate file...");
                                PfsFileEntry cert = pnsp.Files[certIdx];
                                br.BaseStream.Seek(pnsp.HeaderSize + cert.Offset, SeekOrigin.Begin);
                                file = br.ReadBytes((int)cert.Size);
                                usb.Write(file);
                                Log.Log("Certificate was sent to Goldleaf.");
                                break;
                            default:
                                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                                return false;
                        }

                        ccmd = Command.Receive(usb);
                    }

                    return true;
                }
            }
            catch
            {
                Error.Log("An error ocurred opening the selected NSP. Are you sure it's a valid NSP?");
                return false;
            }
        }

        private Pfs ReadPnsp(string filename)
        {
            using (FileStream fs = new FileStream(filename, FileMode.Open))
            using (StreamStorage ist = new StreamStorage(fs, true))
            {
                return new Pfs(ist);
            }
        }

        private (int tikIdx, int certIdx) SendNSPData(Pfs pnsp)
        {
            Command c = new CommandNSPData(pnsp);
            c.Send(usb);

            int tikIdx = -1;
            int certIdx = -1;
            for (int i = 0; i < pnsp.Files.Length; i++)
            {
                string fileExt = Path.GetExtension(pnsp.Files[i].Name).Replace(".", "").ToLower();

                if (fileExt == "tik")
                    tikIdx = i;
                else if (fileExt == "cert")
                    certIdx = i;
            }

            return (tikIdx, certIdx);
        }

        public void SendFinish()
        {
            Command c = new CommandFinish();
            c.Send(usb);
        }
    }
}
