using Goldtree.Lib.Commands;
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

        protected Command(Command other)
        {
            this.Magic = other.Magic;
            this.CommandId = other.CommandId;
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

        public virtual void Send(IUsb usb)
        {
            usb.Write(this);
        }

        public static Command Receive(IUsb usb)
        {
            Command result = usb.Read();
            if (result == null)
                return null;

            switch(result.CommandId)
            {
                case CommandId.ConnectionResponse:
                    return new CommandConnectionResponse(result);
                case CommandId.Finish:
                    return new CommandFinish(result);
                case CommandId.Start:
                    return new CommandStart(result);
                case CommandId.NSPContent:
                    return new CommandNSPContent(result, usb);
                case CommandId.NSPTicket:
                    return new CommandNSPTicket(result);
                case CommandId.NSPCert:
                    return new CommandNSPCert(result);
                default:
                    return result;
            }

        }
    }
}
