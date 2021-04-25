
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

package xorTroll.goldleaf.quark;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Buffer {
    private ByteBuffer buf;

    public Buffer(byte[] data) {
        buf = ByteBuffer.wrap(data);
        buf.order(ByteOrder.LITTLE_ENDIAN);
    }

    public byte read8() {
        return buf.array()[buf.position()];
    }

    public short read16() {
        return buf.getShort();
    }

    public int read32() {
        return buf.getInt();
    }

    public long read64() {
        return buf.getLong();
    }

    public byte[] readBytes(int length) {
        byte[] data = new byte[length];
        buf.get(data);
        return data;
    }

    public void write8(byte val) {
        buf.array()[buf.position()] = val;
    }

    public void write16(short val) {
        buf.putShort(val);
    }

    public void write32(int val) {
        buf.putInt(val);
    }

    public void write64(long val) {
        buf.putLong(val);
    }

    public void writeBytes(byte[] data) {
        buf.put(data);
    }

    public void setPosition(int position) {
        buf.position(position);
    }

    public int getPosition() {
        return buf.position();
    }
}