#include <usb/usb_Commands.hpp>
#include <usb/usb_Communications.hpp>

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

    void InCommandBlock::WriteString(std::string Value)
    {
        Write32(Value.length());
        WriteBuffer((char*)Value.c_str(), Value.length());
    }

    void InCommandBlock::WriteBuffer(void *Buf, size_t Size)
    {
        memcpy(&base.blockbuf[base.position], Buf, Size);
        base.position += Size;
    }

    void InCommandBlock::Send()
    {
        WriteSimple(base.blockbuf, BlockSize);
        operator delete[](base.blockbuf, std::align_val_t(0x1000));
    }

    OutCommandBlock::OutCommandBlock()
    {
        base.position = 0;
        base.blockbuf = new(std::align_val_t(0x1000)) u8[BlockSize]();
        ReadSimple(base.blockbuf, BlockSize);
        magic = Read32();
        res = Read32();
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

    std::string OutCommandBlock::ReadString()
    {
        u32 len = Read32();
        char *str = new char[len + 1]();
        ReadBuffer(str, len);
        std::string nstr(str);
        delete[] str;
        return nstr;
    }

    void OutCommandBlock::ReadBuffer(void *Buf, size_t Size)
    {
        memcpy(Buf, &base.blockbuf[base.position], Size);
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

    InString::InString(std::string Value) : val(Value)
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

    OutString::OutString(std::string &Value) : val(Value)
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
        WriteSimple(alignbuf, sz);
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
        ReadSimple(alignbuf, sz);
        memcpy(buf, alignbuf, sz);
        operator delete[](alignbuf, std::align_val_t(0x1000));
    }
}