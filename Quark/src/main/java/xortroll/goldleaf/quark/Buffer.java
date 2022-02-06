
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

package xortroll.goldleaf.quark;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Buffer {
    private ByteBuffer buf;

    public Buffer(byte[] data) {
        this.buf = ByteBuffer.wrap(data);
        this.buf.order(ByteOrder.LITTLE_ENDIAN);
    }

    public int read32() {
        return this.buf.getInt();
    }

    public long read64() {
        return this.buf.getLong();
    }

    public byte[] readBytes(int length) {
        byte[] data = new byte[length];
        this.buf.get(data);
        return data;
    }

    public void write32(int val) {
        this.buf.putInt(val);
    }

    public void write64(long val) {
        this.buf.putLong(val);
    }

    public void writeBytes(byte[] data) {
        this.buf.put(data);
    }

    public void setPosition(int pos) {
        this.buf.position(pos);
    }

    public int getPosition() {
        return this.buf.position();
    }
}