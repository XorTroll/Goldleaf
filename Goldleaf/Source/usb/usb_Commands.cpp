
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
#include <cstring>

namespace usb
{
    InCommandBlock::InCommandBlock(CommandId CmdId)
    {
        base.position = 0;
        base.blockbuf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        Write32(InputMagic);
        Write32(static_cast<u32>(CmdId));
    }

    void InCommandBlock::Write32(u32 Value)
    {
        WriteBuffer(&Value, sizeof(u32));
    }

    void InCommandBlock::Write64(u64 Value)
    {
        WriteBuffer(&Value, sizeof(u64));
    }

    void InCommandBlock::WriteString(String Value)
    {
        Write32(Value.length());
        WriteBuffer((char16_t*)Value.AsUTF16().c_str(), Value.length() * sizeof(char16_t));
    }

    void InCommandBlock::WriteBuffer(void *Buf, size_t Size)
    {
        memcpy(base.blockbuf + base.position, Buf, Size);
        base.position += Size;
    }

    Result InCommandBlock::Send()
    {
        auto rc = detail::Write(this->base.blockbuf, BlockSize);
        operator delete[](base.blockbuf, std::align_val_t(0x1000));
        return rc;
    }

    OutCommandBlock::OutCommandBlock()
    {
        base.position = 0;
        base.blockbuf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        res = detail::Read(base.blockbuf, BlockSize);
        if(R_SUCCEEDED(res))
        {
            magic = Read32();
            res = Read32();
        }
    }

    void OutCommandBlock::Cleanup()
    {
        operator delete[](base.blockbuf, std::align_val_t(0x1000));
    }

    bool OutCommandBlock::IsValid()
    {
        if(magic != OutputMagic) return false;
        return R_SUCCEEDED(res);
    }

    u32 OutCommandBlock::Read32()
    {
        u32 val = 0;
        ReadBuffer(&val, sizeof(u32));
        return val;
    }

    u64 OutCommandBlock::Read64()
    {
        u64 val = 0;
        ReadBuffer(&val, sizeof(u64));
        return val;
    }

    String OutCommandBlock::ReadString()
    {
        u32 len = Read32();
        auto str = new char16_t[len + 1]();
        ReadBuffer(str, len * sizeof(char16_t));
        String nstr(str);
        delete[] str;
        return nstr;
    }

    void OutCommandBlock::ReadBuffer(void *Buf, size_t Size)
    {
        memcpy(Buf, base.blockbuf + base.position, Size);
        base.position += Size;
    }

    In32::In32(u32 Value) : val(Value)
    {
    }

    void In32::ProcessIn(InCommandBlock &block)
    {
        block.Write32(val);
    }

    void In32::ProcessAfterIn()
    {
    }

    void In32::ProcessOut(OutCommandBlock &block)
    {
    }

    void In32::ProcessAfterOut()
    {
    }

    Out32::Out32(u32 &Value) : val(Value)
    {
    }

    void Out32::ProcessIn(InCommandBlock &block)
    {
    }

    void Out32::ProcessAfterIn()
    {
    }

    void Out32::ProcessOut(OutCommandBlock &block)
    {
        val = block.Read32();
    }

    void Out32::ProcessAfterOut()
    {
    }

    In64::In64(u64 Value) : val(Value)
    {
    }

    void In64::ProcessIn(InCommandBlock &block)
    {
        block.Write64(val);
    }

    void In64::ProcessAfterIn()
    {
    }

    void In64::ProcessOut(OutCommandBlock &block)
    {
    }

    void In64::ProcessAfterOut()
    {
    }

    Out64::Out64(u64 &Value) : val(Value)
    {
    }

    void Out64::ProcessIn(InCommandBlock &block)
    {
    }

    void Out64::ProcessAfterIn()
    {
    }

    void Out64::ProcessOut(OutCommandBlock &block)
    {
        val = block.Read64();
    }

    void Out64::ProcessAfterOut()
    {
    }

    InString::InString(String Value) : val(Value)
    {
    }

    void InString::ProcessIn(InCommandBlock &block)
    {
        block.WriteString(val);
    }

    void InString::ProcessAfterIn()
    {
    }

    void InString::ProcessOut(OutCommandBlock &block)
    {
    }

    void InString::ProcessAfterOut()
    {
    }

    OutString::OutString(String &Value) : val(Value)
    {
    }

    void OutString::ProcessIn(InCommandBlock &block)
    {
    }

    void OutString::ProcessAfterIn()
    {
    }

    void OutString::ProcessOut(OutCommandBlock &block)
    {
        val = block.ReadString();
    }

    void OutString::ProcessAfterOut()
    {
    }

    InBuffer::InBuffer(void *Buf, size_t Sz) : buf(Buf), sz(Sz)
    {
    }

    void InBuffer::ProcessIn(InCommandBlock &block)
    {
    }

    void InBuffer::ProcessAfterIn()
    {
        u8 *alignbuf = new (std::align_val_t(0x1000)) u8[sz]();
        memcpy(alignbuf, buf, sz);
        detail::Write(alignbuf, sz);
        operator delete[](alignbuf, std::align_val_t(0x1000));
    }

    void InBuffer::ProcessOut(OutCommandBlock &block)
    {
    }

    void InBuffer::ProcessAfterOut()
    {
    }

    OutBuffer::OutBuffer(void *Buf, size_t Sz) : buf(Buf), sz(Sz)
    {
    }

    void OutBuffer::ProcessIn(InCommandBlock &block)
    {
    }

    void OutBuffer::ProcessAfterIn()
    {
    }

    void OutBuffer::ProcessOut(OutCommandBlock &block)
    {
    }

    void OutBuffer::ProcessAfterOut()
    {
        u8 *alignbuf = new (std::align_val_t(0x1000)) u8[sz]();
        detail::Read(alignbuf, sz);
        memcpy(buf, alignbuf, sz);
        operator delete[](alignbuf, std::align_val_t(0x1000));
    }
}