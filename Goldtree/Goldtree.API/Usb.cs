using System;
using System.Text;

namespace Goldtree.API
{
    public abstract class Usb
    {
        public abstract byte[] ReadBytes(int Length);
        public abstract void WriteBytes(byte[] Data);

        public byte Read8() => ReadBytes(1)[0];
        public uint Read32() => BitConverter.ToUInt32(ReadBytes(4), 0);
        public ulong Read64() => BitConverter.ToUInt64(ReadBytes(8), 0);
        public string ReadString()
        {
            uint strlen = Read32();
            return Encoding.UTF8.GetString(ReadBytes((int)strlen));
        }
        
        public void Write8(byte Data) => WriteBytes(new byte[] { Data });
        public void Write32(uint Data) => WriteBytes(BitConverter.GetBytes(Data));
        public void Write64(ulong Data) => WriteBytes(BitConverter.GetBytes(Data));
        public void WriteString(string Data)
        {
            Write32((uint)Data.Length);
            WriteBytes(Encoding.ASCII.GetBytes(Data));
        }
    }
}
