using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using libusbK;

namespace Goldtree.Lib.UsbK
{

    public static class UsbKWriteExtras
    {
        public static void Write(this libusbK.UsbK USB, byte[] Data)
        {
            USB.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
        }

        public static void Write(this libusbK.UsbK USB, Command Data)
        {
            USB.Write(Data.AsData());
        }

        public static void Write(this libusbK.UsbK USB, uint Data)
        {
            USB.Write(BitConverter.GetBytes(Data));
        }

        public static void Write(this libusbK.UsbK USB, ulong Data)
        {
            USB.Write(BitConverter.GetBytes(Data));
        }

        public static void Write(this libusbK.UsbK USB, string Data)
        {
            USB.Write(Encoding.UTF8.GetBytes(Data));
        }
    }

    public static class UsbKReadExtras
    {
        public static byte[] Read(this libusbK.UsbK USB, int Length)
        {
            byte[] b = new byte[Length];
            USB.ReadPipe(0x81, b, Length, out _, IntPtr.Zero);
            return b;
        }

        public static Command Read(this libusbK.UsbK USB)
        {
            Command cmd = new Command();
            USB.Read(out uint magic);
            USB.Read(out uint cmdid);
            cmd.Magic = magic;
            cmd.CommandId = (CommandId)cmdid;
            return cmd;
        }

        public static void Read(this libusbK.UsbK USB, out uint Data)
        {
            Data = BitConverter.ToUInt32(USB.Read(4), 0);
        }

        public static void Read(this libusbK.UsbK USB, out ulong Data)
        {
            Data = BitConverter.ToUInt64(USB.Read(8), 0);
        }

        public static void Read(this libusbK.UsbK USB, out string Data, uint Length)
        {
            Data = Encoding.UTF8.GetString(USB.Read((int)Length));
        }
    }

}
