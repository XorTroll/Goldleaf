using Microsoft.AspNetCore.Mvc.Formatters;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree.Usb
{
    public class UsbOutputFormatter : OutputFormatter
    {
        public UsbOutputFormatter()
        {
            SupportedMediaTypes.Add("application/goldleaf");
        }

        protected override bool CanWriteType(Type type)
        {
            return typeof(byte[]).IsAssignableFrom(type)
                || typeof(NSPInfo[]).IsAssignableFrom(type)
                || typeof(string[]).IsAssignableFrom(type);
        }
        public override async Task WriteResponseBodyAsync(OutputFormatterWriteContext context)
        {
            switch(context.Object)
            {
                case byte[] bytes:
                    await context.HttpContext.Response.Body.WriteAsync(bytes, 0, bytes.Length);
                    break;
                case string[] strings:
                    using (BinaryWriter writer = new BinaryWriter(context.HttpContext.Response.Body))
                    {
                        writer.Write(strings.Length);
                        foreach (string str in strings)
                        {
                            writer.Write(Encoding.ASCII.GetByteCount(str));
                            writer.Write(Encoding.ASCII.GetBytes(str));
                        }
                    }
                    break;
                case NSPInfo[] infos:
                    using (BinaryWriter writer = new BinaryWriter(context.HttpContext.Response.Body))
                    {
                        writer.Write(infos.Length);
                        foreach (NSPInfo info in infos)
                        {
                            writer.Write(Encoding.ASCII.GetByteCount(info.Name));
                            writer.Write(Encoding.ASCII.GetBytes(info.Name));
                            writer.Write(info.Offset);
                            writer.Write(info.Size);
                        }
                    }
                    break;
                default:
                    throw new InvalidDataException("Unknown response type");
            }
        }
    }
}
