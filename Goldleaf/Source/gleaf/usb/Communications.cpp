#include <gleaf/usb/Communications.hpp>

namespace gleaf::usb
{
    bool WaitForConnection(u64 Timeout)
    {
        bool ok = true;
        while(true)
        {
            Result rc = usbDsWaitReady(Timeout);
            if(rc == 0) break;
            else if((rc & 0x3FFFFF) != 0xEA01)
            {
                ok = false;
                break;
            }
        }
        return ok;
    }

    size_t Read(void *Out, size_t Size)
    {
        return usbCommsRead(Out, Size);
    }

    size_t ReadFixed(void *Out, size_t Size)
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
        return usbCommsWrite(Buffer, Size);
    }

    size_t WriteFixed(const void *Buffer, size_t Size)
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
}
