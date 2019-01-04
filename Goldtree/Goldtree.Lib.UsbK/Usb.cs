﻿using libusbK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Goldtree.Lib.UsbK
{
    public class Usb : IUsb
    {
        private libusbK.UsbK usb;

        public void Connect(string deviceId)
        {
            KLST_PATTERN_MATCH pat = new KLST_PATTERN_MATCH { DeviceID = deviceId };
            LstK lst = new LstK(0, ref pat);
            lst.MoveNext(out KLST_DEVINFO_HANDLE dinfo);
            usb = new libusbK.UsbK(dinfo);
        }

        public Command Read()
        {
            if (usb == null)
                return null;
            return usb.Read();
        }

        public uint ReadInt32()
        {
            if (usb == null)
                return 0;
            usb.Read(out uint result);
            return result;
        }

        public void Write(Command command)
        {
            if (usb == null)
                return;
            usb.Write(command);
        }

        public void Write(uint value)
        {
            if (usb == null)
                return;
            usb.Write(value);
        }

        public void Write(ulong value)
        {
            if (usb == null)
                return;
            usb.Write(value);
        }

        public void Write(string value)
        {
            if (usb == null)
                return;
            usb.Write(value);
        }

        public void Write(byte[] value)
        {
            if (usb == null)
                return;
            usb.Write(value);
        }
    }
}