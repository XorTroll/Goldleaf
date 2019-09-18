package xorTroll.goldleaf.quark.usb;

import java.util.Optional;
import org.usb4java.*;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class USBInterface
{
    public Context usbContext;
    public Device usbDevice;
    public DeviceHandle usbDeviceHandle;
    public Object usbLock;
    public int usbInterface;

    public static final short VendorId = 0x057E;
    public static final short ProductId = 0x3000;

    public static final byte WriteEndpoint = (byte)0x1;
    public static final byte ReadEndpoint = (byte)0x81;

    private USBInterface(int iface)
    {
        usbInterface = iface;
        usbContext = new Context();
        usbDeviceHandle = null;
        usbDevice = null;
    }

    public synchronized byte[] readBytes(int length)
    {
        ByteBuffer buf = ByteBuffer.allocateDirect(length);
        IntBuffer outlen = IntBuffer.allocate(1);
        int res = LibUsb.bulkTransfer(this.usbDeviceHandle, ReadEndpoint, buf, outlen, 0);
        if(res == LibUsb.SUCCESS)
        {
            int gotlen = outlen.get();
            if(gotlen == length)
            {
                byte[] got = new byte[gotlen];
                buf.get(got);
                return got;
            }
        }
        return null;
    }
    
    public synchronized boolean writeBytes(byte[] data)
    {
        ByteBuffer buf = ByteBuffer.allocateDirect(data.length);
        buf.put(data);
        IntBuffer outlen = IntBuffer.allocate(1);
        int res = LibUsb.bulkTransfer(this.usbDeviceHandle, WriteEndpoint, buf, outlen, 0);
        if(res == LibUsb.SUCCESS) return (outlen.get() == data.length);
        return false;
    }

    public static Optional<USBInterface> createInterface(int iface)
    {
        USBInterface intf = new USBInterface(iface);
        int res = LibUsb.init(intf.usbContext);
        if(res == LibUsb.SUCCESS)
        {
            res = -9999;
            DeviceList devs = new DeviceList();
            int devlist = LibUsb.getDeviceList(intf.usbContext, devs);
            if(devlist > 0)
            {
                for(Device dev: devs)
                {
                    DeviceDescriptor dtor = new DeviceDescriptor();
                    res = LibUsb.getDeviceDescriptor(dev, dtor);
                    if(res == LibUsb.SUCCESS)
                    {
                        if((dtor.idVendor() == VendorId) && (dtor.idProduct() == ProductId))
                        {
                            intf.usbDevice = dev;
                            break;
                        }
                    }
                }

                if(intf.usbDevice != null)
                {
                    intf.usbDeviceHandle = new DeviceHandle();
                    res = LibUsb.open(intf.usbDevice, intf.usbDeviceHandle);
                    if(res == LibUsb.SUCCESS)
                    {
                        LibUsb.freeDeviceList(devs, true);
                        res = LibUsb.setConfiguration(intf.usbDeviceHandle, 1);
                        if(res == LibUsb.SUCCESS)
                        {
                            res = LibUsb.claimInterface(intf.usbDeviceHandle, intf.usbInterface);
                            if(res == LibUsb.SUCCESS) return Optional.of(intf);
                        }
                    }
                }
            }
        }
        return Optional.empty();
    }

    public void finalize()
    {
        if(this.usbDeviceHandle != null)
        {
            LibUsb.releaseInterface(this.usbDeviceHandle, this.usbInterface);
            LibUsb.close(this.usbDeviceHandle);
            LibUsb.exit(this.usbContext);
        }
    }
}