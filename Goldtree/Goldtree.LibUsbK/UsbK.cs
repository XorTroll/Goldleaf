using Goldtree.API;
using libusbK;
using System;

namespace Goldtree.LibUsbK
{
    public class UsbK : Usb
    {
        private readonly UsbKDevice usb;

        public UsbK(int vid, int pid)
        {
            var pat = new KLST_PATTERN_MATCH { DeviceID = $@"USB\VID_{vid:X4}&PID_{pid:X4}" };
            var lst = new LstK(0, ref pat);

            lst.MoveNext(out var dinfo);
            usb = new UsbKDevice(dinfo);
        }

        public override byte[] ReadBytes(int Length)
        {
            byte[] b = new byte[Length];
            usb.ReadPipe(0x81, b, Length, out _, IntPtr.Zero);
            return b;
        }

        public override void WriteBytes(byte[] Data)
        {
            usb.WritePipe(1, Data, Data.Length, out _, IntPtr.Zero);
        }
    }
}
