using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandConnectionResponse : Command
    {
        internal CommandConnectionResponse(Command other) : base(other)
        {
            if (other.CommandId != CommandId.ConnectionResponse)
                throw new ArgumentException("Command must be of type ConnectionResponse", nameof(other));
        }
    }
}
