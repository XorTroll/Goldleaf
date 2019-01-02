using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib.Commands
{
    public class CommandFinish : Command
    {
        public CommandFinish() : base(CommandId.Finish)
        {

        }

        internal CommandFinish(Command other) : base(other)
        {
            if (other.CommandId != CommandId.Finish)
                throw new ArgumentException("Command must be of type Finish", nameof(other));
        }
    }
}
