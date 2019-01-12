using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using LibUsbDotNet;
using LibUsbDotNet.Info;
using LibUsbDotNet.Main;

namespace Goldtree.Lib.LibUsb
{
    public class Usb : IUsb, IDisposable
    {
        private UsbDevice usbDevice;
        private UsbEndpointReader reader;
        private UsbEndpointWriter writer;

        public void Connect(int vid, int pid)
        {
            usbDevice = UsbDevice.OpenUsbDevice(registry =>
                registry.Vid == vid
                && registry.Pid == pid);

            if (usbDevice == null)
                throw new Exception("Could not find device");
            
            IUsbDevice usb = usbDevice as IUsbDevice;
            if (usb != null) //This does some magic to detect if the driver is not winusb
            {
                usb.SetConfiguration(1);
                usb.ClaimInterface(0);
            }

            reader = usbDevice.OpenEndpointReader(ReadEndpointID.Ep01 /* 0x81 */);
            writer = usbDevice.OpenEndpointWriter(WriteEndpointID.Ep01);
        }

        public void Dispose()
        {
            if (usbDevice != null)
                usbDevice.Close();
            UsbDevice.Exit();
        }

        public Command Read()
        {
            if (usbDevice == null)
                return null;

            byte[] data = Read(4);
            if (data == null)
                return null;

            uint magic = BitConverter.ToUInt32(data, 0);

            data = Read(4);
            if (data == null)
                return null;

            uint cmdid = BitConverter.ToUInt32(data, 0);

            return new Command
            {
                Magic = magic,
                CommandId = (CommandId)cmdid
            };
        }

        public uint ReadInt32()
        {
            if (usbDevice == null)
                return 0;

            byte[] data = Read(4);
            if (data == null)
                return 0;

            return BitConverter.ToUInt32(data, 0);
        }

        public void Write(Command command)
        {
            Write(command.Magic);
            Write((uint)command.CommandId);
        }

        public void Write(uint value)
        {
            Write(BitConverter.GetBytes(value));
        }

        public void Write(ulong value)
        {
            Write(BitConverter.GetBytes(value));
        }

        public void Write(string value)
        {
            Write(Encoding.UTF8.GetBytes(value));
        }

        public void Write(byte[] value)
        {
            int offset = 0;
            while (offset < value.Length)
            {
                ErrorCode errorCode = writer.Write(value, offset, value.Length - offset, 1000, out var written);
                if (errorCode != ErrorCode.None)
                    throw new Exception($"Error write writing usb {errorCode}");

                offset += written;
            }
        }

        private byte[] Read(in int count)
        {
            byte[] buffer = new byte[count];
            int offset = 0;
        
            while (offset < count)
            {
                ErrorCode error = reader.Read(buffer, offset, count - offset, 1000, out var read);
                if (error != ErrorCode.None)
                    return null;

                if (read == 0)
                    return null;

                offset += read;
            }

            return buffer;
        }
    }
}
