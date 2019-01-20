using System;
using System.IO;
using System.Text;

namespace Goldtree.Lib
{
    public class Command : IDisposable
    {
        private BinaryReader reader;

        public CommandId CommandId { get; set; }
        public int Size { get; set; }
        public Stream Content { get; set; }

        public Command(CommandId commandId, int size, Stream content)
        {
            CommandId = commandId;
            Size = size;
            Content = content ?? throw new ArgumentNullException(nameof(content));
            reader = new BinaryReader(Content);
        }

        public Command(CommandId commandId, byte[] content)
        {
            CommandId = commandId;
            Size = content.Length;
            Content = new MemoryStream(content);
            reader = new BinaryReader(Content);
        }

        public string ReadString()
        {
            int size = reader.ReadInt32();
            return Encoding.ASCII.GetString(reader.ReadBytes(size));
        }

        public int ReadInt32()
        {
            return reader.ReadInt32();
        }

        public void Dispose()
        {
            Content?.Dispose();
            reader?.Dispose();
        }
    }
}
