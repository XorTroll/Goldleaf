using System;
using System.Collections.Generic;
using System.Text;

namespace Goldtree.Lib
{
    public class Command
    {
        public uint Magic { get; set; }
        public CommandId CommandId { get; set; }

        public static readonly uint GLUC = 0x43554c47;

        public Command()
        {
            Magic = GLUC;
        }

        public Command(CommandId CommandId)
        {
            Magic = GLUC;
            this.CommandId = CommandId;
        }

        public bool MagicOk()
        {
            return (Magic == GLUC);
        }

        public bool IsCommandId(CommandId Id)
        {
            return (CommandId == Id);
        }

        public byte[] AsData()
        {
            List<byte> fcmd = new List<byte>();
            byte[] emg = BitConverter.GetBytes(Magic);
            fcmd.AddRange(emg);
            fcmd.AddRange(BitConverter.GetBytes((uint)CommandId));
            return fcmd.ToArray();
        }
    }
}
