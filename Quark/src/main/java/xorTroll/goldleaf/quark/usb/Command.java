
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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

package xorTroll.goldleaf.quark.usb;

import java.nio.charset.Charset;

import xorTroll.goldleaf.quark.Buffer;
import xorTroll.goldleaf.quark.usb.USBInterface;

public class Command
{
    public enum Id
    {
        Invalid(-1),
        GetDriveCount(0),
        GetDriveInfo(1),
        StatPath(2),
        GetFileCount(3),
        GetFile(4),
        GetDirectoryCount(5),
        GetDirectory(6),
        StartFile(7),
        ReadFile(8),
        WriteFile(9),
        EndFile(10),
        Create(11),
        Delete(12),
        Rename(13),
        GetSpecialPathCount(14),
        GetSpecialPath(15),
        SelectFile(16);

        private int id;

        private Id(int id)
        {
            this.id = id;
        }

        int get32()
        {
            return this.id;
        }

        public boolean compare(int i)
        {
            return id == i;
        }

        public static Id from32(int id)
        {
            Id[] ids = Id.values();
            for(int i = 0; i < ids.length; i++)
            {
                if(ids[i].compare(id)) return ids[i];
            }
            return Id.Invalid;
        }
    }

    public static final int BlockSize = 0x1000;

    public static final int GLCI = 0x49434C47;
    public static final int GLCO = 0x4F434C47;

    private byte[] inner_block;
    private byte[] resp_block;
    private Buffer inner_buf;
    private Buffer resp_buf;
    private USBInterface usbInterface;

    public Command(USBInterface intf)
    {
        usbInterface = intf;
        inner_block = usbInterface.readBytes(BlockSize);
        if(inner_block != null) inner_buf = new Buffer(inner_block);
        resp_block = new byte[BlockSize];
        resp_buf = new Buffer(resp_block);
    }

    public boolean isValid()
    {
        return inner_buf != null;
    }

    public String readString()
    {
        return new String(inner_buf.readBytes(read32() * 2), Charset.forName("UTF_16LE"));
    }

    public int read32()
    {
        return inner_buf.read32();
    }

    public long read64()
    {
        return inner_buf.read64();
    }

    public void write32(int val)
    {
        resp_buf.write32(val);
    }

    public void write64(long val)
    {
        resp_buf.write64(val);
    }

    public void writeString(String val)
    {
        byte[] raw = val.getBytes(Charset.forName("UTF_16LE"));
        write32(val.length());
        resp_buf.writeBytes(raw);
    }

    public void responseStart()
    {
        resp_buf.write32(GLCO);
        resp_buf.write32(0);
    }

    public void sendBuffer(byte[] buf)
    {
        usbInterface.writeBytes(buf);
    }

    public byte[] getBuffer(int len)
    {
        return usbInterface.readBytes(len);
    }

    public void responseEnd()
    {
        usbInterface.writeBytes(resp_block);
    }

    public void respondFailure(int result)
    {
        resp_buf.write32(GLCO);
        resp_buf.write32(result);
        responseEnd();
    }

    public void respondEmpty()
    {
        responseStart();
        responseEnd();
    }
}