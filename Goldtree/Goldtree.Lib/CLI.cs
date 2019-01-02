using gtree;
using LibHac;
using LibHac.IO;
using libusbK;
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


        private static CLI instance;

        public static CLI Instance
        {
            get
            {
                if (instance == null)
                    instance = new CLI();
                return instance;
            }
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
            Warn.Log("Make sure to open Goldtree after having launched Goldleaf on your console. Waiting for connection...");
        }

        public UsbK GetUsb()
        {
            try
            {
                KLST_PATTERN_MATCH pat = new KLST_PATTERN_MATCH { DeviceID = @"USB\VID_057E&PID_3000" };
                LstK lst = new LstK(0, ref pat);
                lst.MoveNext(out KLST_DEVINFO_HANDLE dinfo);
                return new UsbK(dinfo);
            }
            catch
            {
                Error.Log("No USB connection was found. Make sure you have Goldleaf open before running Goldtree.");
                return null;
            }
        }

        public bool ConnectToGoldleaf(UsbK Usb)
        {
            Command c = new Command(CommandId.ConnectionRequest);
            Usb.Write(c);
            Log.Log("Attempting to connect to Goldleaf via USB...");
            Command rc = Usb.Read();

            if (!rc.MagicOk())
            {
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                return false;
            }

            if (rc.IsCommandId(CommandId.ConnectionResponse))
                return true;
            else if (rc.IsCommandId(CommandId.Finish))
                Error.Log("Goldleaf has canceled the installation.");
            else
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");

            return false;
        }

        public bool SendFileName(UsbK usb, string file)
        {
            string nspname = Path.GetFileName(file);
            Command c = new Command(CommandId.NSPName);
            usb.Write(c);
            usb.Write((uint)nspname.Length);
            usb.Write(nspname);
            Log.Log("Selected NSP's name was sent to Goldleaf. Waiting for install approval from Goldleaf...");
            Command rc2 = usb.Read();
            if (!rc2.MagicOk())
            {
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                return false;
            }

            if (rc2.IsCommandId(CommandId.Start))
            {
                Log.Log("Goldleaf is ready for the installation. Preparing everything...");
                return true;
            }
            else if (rc2.IsCommandId(CommandId.Finish))
            {
                Error.Log("Goldleaf has canceled the installation.");
                return false;
            }
            else
            {
                Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                return false;
            }
        }

        public bool SendFileContent(UsbK usb, string fileName)
        {
            Log.Log("Goldleaf is ready for the installation. Preparing everything...");
            try
            {
                Pfs pnsp;

                using (FileStream fs = new FileStream(fileName, FileMode.Open))
                using (StreamStorage ist = new StreamStorage(fs, true))
                {
                    pnsp = new Pfs(ist);
                }

                var (tikIdx, certIdx) = SendNSPData(usb, pnsp);

                using (FileStream fs = new FileStream(fileName, FileMode.Open))
                using (BinaryReader br = new BinaryReader(fs))
                {
                    Command ccmd = usb.Read();
                    while(!ccmd.MagicOk() || ccmd.IsCommandId(CommandId.Finish))
                    {
                        if (!ccmd.MagicOk())
                        {
                            Error.Log("An invalid command was received. Are you sure Goldleaf is active?");
                            return false;
                        }

                        switch (ccmd.CommandId)
                        {
                            case CommandId.NSPContent:
                                usb.Read(out uint idx);
                                Log.Log("Sending content \'" + pnsp.Files[idx].Name + "\'... (" + (idx + 1) + " of " + pnsp.Files.Length + ")");
                                PfsFileEntry ent = pnsp.Files[idx];
                                long rsize = 1048576;
                                long coffset = pnsp.HeaderSize + ent.Offset;
                                long toread = ent.Size;
                                long tmpread = 1;
                                byte[] bufb;
                                while ((tmpread > 0) && (toread > 0) && (coffset < (coffset + ent.Size)))
                                {
                                    if (rsize >= ent.Size) rsize = ent.Size;
                                    int tor = (int)Math.Min(rsize, toread);
                                    bufb = new byte[tor];
                                    br.BaseStream.Position = coffset;
                                    tmpread = br.Read(bufb, 0, bufb.Length);
                                    usb.Write(bufb);
                                    coffset += tmpread;
                                    toread -= tmpread;
                                }
                                Log.Log("Content was sent to Goldleaf.");
                                break;
                            case CommandId.NSPTicket:
                                Log.Log("Sending ticket file...");
                                PfsFileEntry tik = pnsp.Files[tikIdx];
                                br.BaseStream.Seek(pnsp.HeaderSize + tik.Offset, SeekOrigin.Begin);
                                byte[] file = br.ReadBytes((int)tik.Size);
                                usb.Write(file);
                                Log.Log("Ticket was sent to Goldleaf.");
                                break;
                            case CommandId.NSPCert:
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

                        ccmd = usb.Read();
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

        private (int tikIdx, int certIdx) SendNSPData(UsbK usb, Pfs pnsp)
        {
            uint filecount = (uint)pnsp.Files.Length;
            Command c = new Command(CommandId.NSPData);
            usb.Write(c);
            usb.Write(filecount);
            int tikIdx = -1;
            int certIdx = -1;
            for (int i = 0; i < pnsp.Files.Length; i++)
            {
                PfsFileEntry file = pnsp.Files[i];

                ulong offset = (ulong)pnsp.HeaderSize + (ulong)file.Offset;
                ulong size = (ulong)file.Size;
                uint len = (uint)file.Name.Length;
                usb.Write(len);
                usb.Write(file.Name);
                usb.Write(offset);
                usb.Write(size);

                if (Path.GetExtension(file.Name).Replace(".", "").ToLower() == "tik")
                    tikIdx = i;
                else if (Path.GetExtension(file.Name).Replace(".", "").ToLower() == "cert")
                    certIdx = i;
            }

            return (tikIdx, certIdx);
        }

        public void SendFinish(UsbK usb)
        {
            Command c = new Command(CommandId.Finish);
            usb.Write(c);
        }
    }
}
