
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

package xortroll.goldleaf.quark.usb.cmd;

import java.nio.charset.StandardCharsets;
import xortroll.goldleaf.quark.Buffer;
import xortroll.goldleaf.quark.usb.USBInterface;

public class CommandBlock {
    public static final int BlockSize = 0x1000;

    public static final int InputMagic = 0x49434C47; // 'GLCI'
    public static final int OutputMagic = 0x4F434C47; // 'GLCO'

    public static final int ResultSuccess = 0;

    public static final int InvalidCommandId = 0;

    private byte[] inner_block;
    private byte[] resp_block;
    private Buffer inner_buf;
    private Buffer resp_buf;
    private USBInterface usb_intf;

    public CommandBlock(USBInterface intf) {
        this.usb_intf = intf;
        this.inner_block = usb_intf.readBytes(BlockSize);
        if(this.inner_block != null) {
            this.inner_buf = new Buffer(this.inner_block);
        }
        this.resp_block = new byte[BlockSize];
        this.resp_buf = new Buffer(this.resp_block);
    }

    public boolean isValid() {
        return this.inner_buf != null;
    }

    public String readString() {
        int str_len = this.read32();
        return new String(this.inner_buf.readBytes(str_len), StandardCharsets.UTF_8);
    }

    public int read32() {
        return this.inner_buf.read32();
    }

    public long read64() {
        return this.inner_buf.read64();
    }

    public void write32(int val) {
        this.resp_buf.write32(val);
    }

    public void write64(long val) {
        this.resp_buf.write64(val);
    }

    public void writeString(String val) {
        byte[] raw = val.getBytes(StandardCharsets.UTF_8);
        this.write32(raw.length);
        this.resp_buf.writeBytes(raw);
    }

    public void sendBuffer(byte[] buf) {
        this.usb_intf.writeBytes(buf);
    }

    public byte[] getBuffer(int len) {
        return this.usb_intf.readBytes(len);
    }

    public int validateCommand() {
        int input_magic = this.read32();
        if(input_magic == InputMagic) {
            int cmd_id = this.read32();
            return cmd_id;
        }
        else {
            return InvalidCommandId;
        }
    }

    public void responseStart() {
        this.resp_buf.write32(OutputMagic);
        this.resp_buf.write32(ResultSuccess);
    }

    public void responseEnd() {
        this.usb_intf.writeBytes(this.resp_block);
    }

    public void respondFailure(int rc) {
        resp_buf.write32(OutputMagic);
        resp_buf.write32(rc);
        responseEnd();
    }

    public void respondEmpty() {
        responseStart();
        responseEnd();
    }
}