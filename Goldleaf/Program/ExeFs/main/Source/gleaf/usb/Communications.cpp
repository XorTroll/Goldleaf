#include <gleaf/usb/Communications.hpp>
#include <malloc.h>

namespace gleaf::usb
{
    size_t Read(void *Out, size_t Size, VoidFn Callback)
    {
        size_t read = 0;
        usbCommsRead(Out, Size, &read, Callback);
        return read;
    }

    Command ReadCommand(VoidFn Callback)
    {
        void *data = memalign(0x1000, sizeof(Command));
        Read(data, sizeof(Command), Callback);
        Command cmd = *(Command*)data;
        free(data);
        return cmd;
    }

    u32 Read32()
    {
        void *data = memalign(0x1000, sizeof(u32));
        Read(data, sizeof(u32));
        u32 num = *(u32*)data;
        free(data);
        return num;
    }

    u64 Read64()
    {
        void *data = memalign(0x1000, sizeof(u64));
        Read(data, sizeof(u64));
        u64 num = *(u64*)data;
        free(data);
        return num;
    }

    std::string ReadString(u32 Length)
    {
        void *data = memalign(0x1000, Length + 1);
        Read(data, Length);
        char *cdata = reinterpret_cast<char*>(data);
        cdata[Length] = '\0';
        std::string str = std::string(cdata);
        free(cdata);
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

    u32 GetState()
    {
        u32 st = 0;
        usbDsGetState(&st);
        return st;
    }

    bool IsStatePlugged()
    {
        return (GetState() == 5);
    }

    bool IsStateNotReady()
    {
        return !IsStatePlugged();
    }
}
