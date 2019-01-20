using Goldtree.Lib;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree.Usb
{
    class RequestBuilder
    {
        private readonly IUsb usb;

        public RequestBuilder(in IUsb usb)
        {
            this.usb = usb ?? throw new ArgumentNullException(nameof(usb));
        }

        public RequestFeature Build()
        {
            Command cmd = ReadCommand();

            switch(cmd.CommandId)
            {
                case CommandId.Info:
                    return new RequestFeature("GET", "/info", cmd.Content);
                case CommandId.ListNSPs:
                    return new RequestFeature("GET", "/files", new MemoryStream());
                case CommandId.GetNSPInfo:
                    return new RequestFeature("GET", $"/file/{cmd.ReadString()}/info", new MemoryStream());
                case CommandId.GetNSPContent:
                    return new RequestFeature("GET", $"/file/{cmd.ReadString()}/content/{cmd.ReadInt32()}", new MemoryStream());
                case CommandId.GetNSPTicket:
                    return new RequestFeature("GET", $"/file/{cmd.ReadString()}/ticket", new MemoryStream());
                case CommandId.GetNSPCertificate:
                    return new RequestFeature("GET", $"/file/{cmd.ReadString()}/certificate", new MemoryStream());
                case CommandId.Echo:
                    return new RequestFeature("POST", "/echo", cmd.Content);
                default:
                    throw new InvalidDataException($"Received unknown command: {cmd.CommandId}");
            }
        }

        private Command ReadCommand()
        {
            CommandId id = (CommandId)usb.ReadInt32();
            uint size = usb.ReadInt32();
            byte[] content = usb.ReadBytes(size);

            return new Command(id, content);
        }
    }
}
