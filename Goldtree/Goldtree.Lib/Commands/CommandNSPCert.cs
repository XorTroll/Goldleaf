using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandNSPCert : Command
    {
        internal CommandNSPCert(Command other) : base(other)
        {
            if (other.CommandId != CommandId.NSPCert)
                throw new ArgumentException("Command must be of type NSPCert", nameof(other));
        }
    }
}
