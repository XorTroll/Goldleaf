
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

#include <usb/cmd/cmd_Base.hpp>

namespace usb::cmd {

    InCommandBlock::InCommandBlock(const u32 cmd_id) {
        this->base.position = 0;
        this->ok = true;
        this->base.block_buf = new (std::align_val_t(0x1000)) u8[BlockSize]();
        
        if(!this->WriteValue(InputMagic)) {
            this->ok = false;
        }

        if(!this->WriteValue(cmd_id)) {
            this->ok = false;
        }
    }

    bool InCommandBlock::WriteString(const std::string &val) {
        const auto len = val.length();
        
        if(!this->WriteValue(static_cast<u32>(len))) {
            return false;
        }

        if(!this->WriteBuffer(val.c_str(), len)) {
            return false;
        }

        return true;
    }

    bool InCommandBlock::WriteBuffer(const void *buf, const size_t size) {
        if((this->base.position + size) > BlockSize) {
            return false;
        }
        else {
            memcpy(this->base.block_buf + this->base.position, buf, size);
            this->base.position += size;
            return true;
        }
    }

    Result InCommandBlock::Send() {
        const auto rc = Write(this->base.block_buf, BlockSize);
        operator delete[](this->base.block_buf, std::align_val_t(0x1000));
        this->base.block_buf = nullptr;
        return rc;
    }

    OutCommandBlock::OutCommandBlock() {
        this->base.position = 0;
        this->base.block_buf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        this->res = Read(this->base.block_buf, BlockSize);
        if(R_SUCCEEDED(this->res)) {
            if(!this->ReadValue(this->magic)) {
                this->res = 0xBABA;
            }
            if(!this->ReadValue(this->res)) {
                this->res = 0xBABA;
            }
        }
    }

    void OutCommandBlock::Cleanup() {
        if(this->base.block_buf != nullptr) {
            operator delete[](this->base.block_buf, std::align_val_t(0x1000));
            this->base.block_buf = nullptr;
        }
    }

    bool OutCommandBlock::ReadString(std::string &out_str) {
        u32 str_len = 0;
        if(!this->ReadValue(str_len)) {
            return false;
        }

        auto str_buf = new char[str_len + 1]();
        if(!this->ReadBuffer(str_buf, str_len)) {
            return false;
        }
    
        out_str.assign(str_buf);
        delete[] str_buf;
        return true;
    }

    bool OutCommandBlock::ReadBuffer(void *buf, const size_t size) {
        if((this->base.position + size) > BlockSize) {
            return false;
        }
        else {
            memcpy(buf, this->base.block_buf + this->base.position, size);
            this->base.position += size;
            return true;
        }
    }

    bool InBuffer::ProcessAfterIn() {
        auto aligned_buf = new (std::align_val_t(0x1000)) u8[this->size]();
        memcpy(aligned_buf, this->buf, this->size);

        const auto ok = R_SUCCEEDED(Write(aligned_buf, this->size));
        operator delete[](aligned_buf, std::align_val_t(0x1000));
        return ok;
    }

    bool OutBuffer::ProcessAfterOut() {
        auto aligned_buf = new (std::align_val_t(0x1000)) u8[this->size]();

        const auto ok = R_SUCCEEDED(Read(aligned_buf, this->size));
        memcpy(this->buf, aligned_buf, this->size);
        operator delete[](aligned_buf, std::align_val_t(0x1000));
        return ok;
    }

}