using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using libusbK;

namespace gleaf.tree
{
    public enum ReceiveCommand
    {
        Exit,
    }

    public static class UsbKExtra
    {
        public static void Write(this UsbK USB, byte[] Data)
        {
            USB.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
        }

        public static byte[] Read(this UsbK USB, int Length)
        {
            byte[] b = new byte[Length];
            USB.ReadPipe(1, b, Length, out _, IntPtr.Zero);
            return b;
        }
    }

    public static class Commands
    {
        public static void ReceiveCommand(UsbK USB)
        {
            byte[] cmd = USB.Read(8);
            byte[] mg = new byte[] { cmd[0], cmd[1], cmd[2], cmd[3] };
            byte cmdid = cmd[4];

        }

        public static void SendCommand(string NSP)
        {
            
        }
    }
}
