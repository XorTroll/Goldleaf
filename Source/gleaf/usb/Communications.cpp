#include <gleaf/USB/Communications.hpp>

namespace gleaf::usb
{
    size_t Read(void *Out, size_t Size)
    {
        u8 *buf = (u8*)Out;
        size_t szrem = Size;
        size_t rsize = 0;
        while(szrem)
        {
            rsize = usbCommsRead(buf, szrem);
            buf += rsize;
            szrem -= rsize;
        }
        return Size;
    }

    size_t Write(const void *Buffer, size_t Size)
    {
        const u8 *bufptr = (const u8*)Buffer;
        size_t sz = Size;
        size_t tsz = 0;
        while(sz)
        {
            tsz = usbCommsWrite(bufptr, sz);
            bufptr += tsz;
            sz -= tsz;
        }
        return Size;
    }

    void SendCommandHeader(u32 CommandId, u64 DataSize)
    {
        CommandHeader header;
        header.Magic = usb::GLUC;
        header.Type = static_cast<u8>(CommandType::Request);
        header.CommandId = CommandId;
        header.DataSize = DataSize;
        Write(&header, sizeof(CommandHeader));
    }

    void SendExitCommand()
    {
        SendCommandHeader(0, 0);
    }
}