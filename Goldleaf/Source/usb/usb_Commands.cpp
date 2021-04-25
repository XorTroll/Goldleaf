
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

#include <usb/usb_Commands.hpp>

namespace usb {

    InCommandBlock::InCommandBlock(CommandId cmd_id) {
        this->base.position = 0;
        this->base.block_buf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        Write32(InputMagic);
        Write32(static_cast<u32>(cmd_id));
    }

    void InCommandBlock::Write32(u32 val) {
        WriteBuffer(&val, sizeof(u32));
    }

    void InCommandBlock::Write64(u64 val) {
        WriteBuffer(&val, sizeof(u64));
    }

    void InCommandBlock::WriteString(String val) {
        const auto len = val.length();
        Write32(len);
        WriteBuffer(val.AsUTF16().c_str(), sizeof(char16_t) * len);
    }

    void InCommandBlock::WriteBuffer(const void *buf, size_t size) {
        memcpy(this->base.block_buf + this->base.position, buf, size);
        this->base.position += size;
    }

    Result InCommandBlock::Send() {
        const auto rc = detail::Write(this->base.block_buf, BlockSize);
        operator delete[](this->base.block_buf, std::align_val_t(0x1000));
        return rc;
    }

    OutCommandBlock::OutCommandBlock() {
        this->base.position = 0;
        this->base.block_buf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        this->res = detail::Read(this->base.block_buf, BlockSize);
        if(R_SUCCEEDED(this->res)) {
            this->magic = Read32();
            this->res = Read32();
        }
    }

    void OutCommandBlock::Cleanup() {
        operator delete[](this->base.block_buf, std::align_val_t(0x1000));
    }

    bool OutCommandBlock::IsValid() {
        if(this->magic != OutputMagic) {
            return false;
        }
        return R_SUCCEEDED(this->res);
    }

    u32 OutCommandBlock::Read32() {
        u32 val = 0;
        ReadBuffer(&val, sizeof(val));
        return val;
    }

    u64 OutCommandBlock::Read64() {
        u64 val = 0;
        ReadBuffer(&val, sizeof(val));
        return val;
    }

    String OutCommandBlock::ReadString() {
        const auto len = Read32();
        auto str_buf = new char16_t[len + 1]();
        ReadBuffer(str_buf, len * sizeof(char16_t));
    
        String str = str_buf;
        delete[] str_buf;
        return str;
    }

    void OutCommandBlock::ReadBuffer(void *buf, size_t size) {
        memcpy(buf, this->base.block_buf + this->base.position, size);
        this->base.position += size;
    }

    In32::In32(u32 val) : val(val) {}

    void In32::ProcessIn(InCommandBlock &block) {
        block.Write32(this->val);
    }

    void In32::ProcessAfterIn() {}
    void In32::ProcessOut(OutCommandBlock &block) {}
    void In32::ProcessAfterOut() {}

    Out32::Out32(u32 &val) : val(val) {}
    void Out32::ProcessIn(InCommandBlock &block) {}
    void Out32::ProcessAfterIn() {}

    void Out32::ProcessOut(OutCommandBlock &block) {
        this->val = block.Read32();
    }

    void Out32::ProcessAfterOut() {}

    In64::In64(u64 val) : val(val) {}

    void In64::ProcessIn(InCommandBlock &block) {
        block.Write64(this->val);
    }

    void In64::ProcessAfterIn() {}
    void In64::ProcessOut(OutCommandBlock &block) {}
    void In64::ProcessAfterOut() {}

    Out64::Out64(u64 &val) : val(val) { }
    void Out64::ProcessIn(InCommandBlock &block) {}
    void Out64::ProcessAfterIn() {}

    void Out64::ProcessOut(OutCommandBlock &block) {
        this->val = block.Read64();
    }

    void Out64::ProcessAfterOut() {}

    InString::InString(String val) : val(val) {}

    void InString::ProcessIn(InCommandBlock &block) {
        block.WriteString(this->val);
    }

    void InString::ProcessAfterIn() {}
    void InString::ProcessOut(OutCommandBlock &block) {}
    void InString::ProcessAfterOut() {}

    OutString::OutString(String &val) : val(val) {}
    void OutString::ProcessIn(InCommandBlock &block) {}
    void OutString::ProcessAfterIn() {}

    void OutString::ProcessOut(OutCommandBlock &block) {
        this->val = block.ReadString();
    }

    void OutString::ProcessAfterOut() {}

    InBuffer::InBuffer(const void *buf, size_t size) : buf(buf), size(size) {}
    void InBuffer::ProcessIn(InCommandBlock &block) {}

    void InBuffer::ProcessAfterIn() {
        auto aligned_buf = new (std::align_val_t(0x1000)) u8[this->size]();
        memcpy(aligned_buf, this->buf, this->size);
        detail::Write(aligned_buf, this->size);
        operator delete[](aligned_buf, std::align_val_t(0x1000));
    }

    void InBuffer::ProcessOut(OutCommandBlock &block) {}
    void InBuffer::ProcessAfterOut() {}

    OutBuffer::OutBuffer(void *buf, size_t size) : buf(buf), size(size) {}
    void OutBuffer::ProcessIn(InCommandBlock &block) {}
    void OutBuffer::ProcessAfterIn() {}
    void OutBuffer::ProcessOut(OutCommandBlock &block) {}

    void OutBuffer::ProcessAfterOut() {
        auto aligned_buf = new (std::align_val_t(0x1000)) u8[this->size]();
        detail::Read(aligned_buf, this->size);
        memcpy(this->buf, aligned_buf, this->size);
        operator delete[](aligned_buf, std::align_val_t(0x1000));
    }

}