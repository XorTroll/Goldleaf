using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandNSPName : Command
    {
        public string Name { get; }

        public CommandNSPName(string name) : base(CommandId.NSPName)
        {
            Name = name;
        }

        public override void Send(IUsb usb)
        {
            base.Send(usb);

            usb.Write((uint)Name.Length);
            usb.Write(Name);
        }
    }
}
