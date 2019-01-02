using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandStart : Command
    {
        internal CommandStart(Command other) : base(other)
        {
            if (other.CommandId != CommandId.Start)
                throw new ArgumentException("Command must be of type Start", nameof(other));
        }
    }
}
