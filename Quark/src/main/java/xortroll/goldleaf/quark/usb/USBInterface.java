
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

package xortroll.goldleaf.quark.usb;

import java.util.Optional;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import org.usb4java.*;
import xortroll.goldleaf.quark.Logging;
import xortroll.goldleaf.quark.Version;

public class USBInterface {
    public static final short VendorId = 0x057E;
    public static final short ProductId = 0x3000;

    public static final byte WriteEndpoint = (byte)0x1;
    public static final byte ReadEndpoint = (byte)0x81;

    private int interface_no;
    private Context context;
    private Device device;
    private DeviceHandle device_handle;
    private boolean is_dev_version;
    private Version product_version;

    private USBInterface(int interface_no) {
        this.interface_no = interface_no;
        this.context = new Context();
        this.device_handle = null;
        this.device = null;
    }

    public boolean isDevVersion() {
        return this.is_dev_version;
    }

    public Version getProductVersion() {
        return this.product_version;
    }

    private synchronized boolean readImpl(ByteBuffer buf, int size) {
        IntBuffer out_read_size_buf = IntBuffer.allocate(1);
        int res = LibUsb.bulkTransfer(this.device_handle, ReadEndpoint, buf, out_read_size_buf, 0);
        int out_read_size = out_read_size_buf.get(0);
        return (res == LibUsb.SUCCESS) && (out_read_size == size);
    }

    private synchronized boolean writeImpl(ByteBuffer buf, int size) {
        IntBuffer out_write_size_buf = IntBuffer.allocate(1);
        int res = LibUsb.bulkTransfer(this.device_handle, WriteEndpoint, buf, out_write_size_buf, 0);
        int out_write_size = out_write_size_buf.get(0);
        return (res == LibUsb.SUCCESS) && (out_write_size == size);
    }

    public byte[] readBytes(int length) {
        ByteBuffer buf = ByteBuffer.allocateDirect(length);
        buf.order(ByteOrder.LITTLE_ENDIAN);
        this.readImpl(buf, length);
        byte[] data = new byte[length];
        buf.get(data);
        return data;
    }

    public boolean writeBytes(byte[] data) {
        ByteBuffer buf = ByteBuffer.allocateDirect(data.length);
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.put(data);
        return this.writeImpl(buf, data.length);
    }

    public static Optional<USBInterface> createInterface(int interface_no) {
        USBInterface intf = new USBInterface(interface_no);
        int res = LibUsb.init(intf.context);
        if(res == LibUsb.SUCCESS) {
            DeviceList devices = new DeviceList();
            int device_count = LibUsb.getDeviceList(intf.context, devices);
            if(device_count > 0) {
                for(Device dev: devices) {
                    DeviceDescriptor device_desc = new DeviceDescriptor();
                    res = LibUsb.getDeviceDescriptor(dev, device_desc);
                    if(res == LibUsb.SUCCESS) {
                        if((device_desc.idVendor() == VendorId) && (device_desc.idProduct() == ProductId)) {
                            intf.device = dev;
                            break;
                        }
                    }
                }

                if(intf.device != null) {
                    intf.device_handle = new DeviceHandle();
                    res = LibUsb.open(intf.device, intf.device_handle);
                    if(res == LibUsb.SUCCESS) {
                        LibUsb.freeDeviceList(devices, true);
                        res = LibUsb.setConfiguration(intf.device_handle, 1);
                        if(res == LibUsb.SUCCESS) {
                            intf.is_dev_version = false;
                            intf.product_version = null;
                            DeviceDescriptor device_desc = new DeviceDescriptor();
                            res = LibUsb.getDeviceDescriptor(intf.device, device_desc);
                            if(res == LibUsb.SUCCESS) {
                                String product = LibUsb.getStringDescriptor(intf.device_handle, device_desc.iProduct());
                                Logging.log("USB Product: '" + product + "'");
                                if(product.contains("Goldleaf")) {
                                    String serial_no = LibUsb.getStringDescriptor(intf.device_handle, device_desc.iSerialNumber());
                                    Logging.log("USB Serial number: '" + serial_no + "'");
                                    if(serial_no.endsWith("-dev")) {
                                        intf.is_dev_version = true;
                                        serial_no = serial_no.substring(0, serial_no.length() - 4);
                                    }
                                    try {
                                        String[] version_tokens = serial_no.split("[.]");
                                        if(version_tokens.length >= 2) {
                                            int major = Integer.parseInt(version_tokens[0]);
                                            int minor = Integer.parseInt(version_tokens[1]);
                                            int micro = 0;
                                            if(version_tokens.length >= 3) {
                                                micro = Integer.parseInt(version_tokens[2]);
                                            }
                                            intf.product_version = new Version(major, minor, micro);
                                        }
                                    }
                                    catch(Exception e) {}
                                }
                            }

                            res = LibUsb.claimInterface(intf.device_handle, intf.interface_no);
                            if(res == LibUsb.SUCCESS) {
                                return Optional.of(intf);
                            }
                        }
                    }
                }
            }
        }
        return Optional.empty();
    }

    public void finalize() {
        if(this.device_handle != null) {
            LibUsb.releaseInterface(this.device_handle, this.interface_no);
            LibUsb.close(this.device_handle);
            LibUsb.exit(this.context);
            this.device_handle = null;
        }
    }
}