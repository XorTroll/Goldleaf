using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandNSPContent : Command
    {
        public uint Index { get; }
        internal CommandNSPContent(Command other, IUsb usb) : base(other)
        {
            if (other.CommandId != CommandId.NSPContent)
                throw new ArgumentException("Command must be of type NSPContent", nameof(other));

            Index = usb.ReadInt32();
        }
    }
}
