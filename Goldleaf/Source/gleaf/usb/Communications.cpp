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

    Command ReadCommand()
    {
        Command cmd;
        ReadFixed(&cmd, sizeof(Command));
        return cmd;
    }

    u32 Read32()
    {
        u32 data;
        Read((void*)&data, sizeof(u32));
        return data;
    }

    u64 Read64()
    {
        u64 data;
        Read((void*)&data, sizeof(u64));
        return data;
    }

    std::string ReadString(u32 Length)
    {
        u8 *data = (u8*)malloc(sizeof(u8) * Length);
        Read((void*)data, (sizeof(u8) * Length));
        std::string str = (const char*)data;
        return str;
    }

    size_t ReadFixed(void *Out, size_t Size)
    {
        u8 *buf = (u8*)Out;
        size_t szrem = Size;
        size_t rsize = 0;
        while(szrem)
        {
            rsize = Read(buf, szrem);
            buf += rsize;
            szrem -= rsize;
        }
        return Size;
    }

    size_t Write(const void *Buffer, size_t Size)
    {
        return usbCommsWrite(Buffer, Size);
    }

    void WriteCommand(Command &Data)
    {
        WriteFixed(&Data, sizeof(Command));
    }

    void Write32(u32 Data)
    {
        Write((void*)&Data, sizeof(u32));
    }

    void Write64(u64 Data)
    {
        Write((void*)&Data, sizeof(u64));
    }

    void WriteString(std::string Data)
    {
        Write((void*)Data.c_str(), (sizeof(u8) * Data.length()));
    }

    size_t WriteFixed(const void *Buffer, size_t Size)
    {
        const u8 *bufptr = (const u8*)Buffer;
        size_t sz = Size;
        size_t tsz = 0;
        while(sz)
        {
            tsz = Write(bufptr, sz);
            bufptr += tsz;
            sz -= tsz;
        }
        return Size;
    }
}
