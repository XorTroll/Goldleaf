using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandNSPTicket : Command
    {
        internal CommandNSPTicket(Command other) : base(other)
        {
            if (other.CommandId != CommandId.NSPTicket)
                throw new ArgumentException("Command must be of type NSPTicket", nameof(other));
        }
    }
}
