using LibHac;
using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandNSPData : Command
    {
        public Pfs Pnsp { get; }
        public CommandNSPData(Pfs pnsp) : base(CommandId.NSPData)
        {
            Pnsp = pnsp;
        }

        public override void Send(IUsb usb)
        {
            base.Send(usb);

            uint filecount = (uint)Pnsp.Files.Length;
            usb.Write(filecount);

            foreach (PfsFileEntry file in Pnsp.Files)
            {
                ulong offset = (ulong)Pnsp.HeaderSize + (ulong)file.Offset;
                ulong size = (ulong)file.Size;
                uint len = (uint)file.Name.Length;
                usb.Write(len);
                usb.Write(file.Name);
                usb.Write(offset);
                usb.Write(size);
            }
        }
    }
}
