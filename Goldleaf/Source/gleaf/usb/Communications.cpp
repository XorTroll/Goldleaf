#include <gleaf/usb/Communications.hpp>
#include <malloc.h>

namespace gleaf::usb
{
    size_t ReadSimple(void *Out, size_t Size, UsbCallbackFn LoopCallback)
    {
        size_t sz = 0;
        usbCommsRead(Out, Size, &sz, LoopCallback);
        return sz;
    }

    size_t WriteSimple(const void *Buffer, size_t Size)
    {
        return usbCommsWrite(Buffer, Size);
    }

    size_t Read(void *Out, size_t Size, UsbCallbackFn LoopCallback)
    {
        u8 *bufptr = (u8*)Out;
        size_t sz = Size;
        size_t tsz = 0;
        Result rc = 0;
        while(sz)
        {
            rc = usbCommsRead(bufptr, sz, &tsz, LoopCallback);
            if(rc != 0) return 0;
            if(tsz == 0) return 0;
            bufptr += tsz;
            sz -= tsz;
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
            if(tsz == 0) return 0;
            bufptr += tsz;
            sz -= tsz;
        }
        return Size;
    }

    u8 Read8(UsbCallbackFn Callback)
    {
        u8 num = 0;
        Read(&num, sizeof(u8), Callback);
        return num;
    }

    u32 Read32(UsbCallbackFn Callback)
    {
        u32 num = 0;
        Read(&num, sizeof(u32), Callback);
        return num;
    }

    u64 Read64(UsbCallbackFn Callback)
    {
        u64 num = 0;
        Read(&num, sizeof(u64), Callback);
        return num;
    }

    std::string ReadString(UsbCallbackFn Callback)
    {
        u32 len = Read32(Callback);
        if(len == 0) return "";
        char *data = (char*)memalign(0x1000, len + 1);
        if(Read(data, len, Callback) == 0) return "";
        data[len] = '\0';
        std::string str = std::string(data);
        return str;
    }

    bool WriteCommand(CommandId Id)
    {
        bool ok = (Write(&GLUC, sizeof(u32)) != 0);
        if(ok)
        {
            u32 cmdid = static_cast<u32>(Id);
            ok = (Write(&cmdid, sizeof(u32)) != 0);
        }
        return ok;
    }

    bool Write8(u8 Data)
    {
        return (Write(&Data, 1) != 0);
    }

    bool Write32(u32 Data)
    {
        return (Write((u8*)&Data, sizeof(u32)) != 0);
    }

    bool Write64(u64 Data)
    {
        return (Write((u8*)&Data, sizeof(u64)) != 0);
    }

    bool WriteString(std::string Str)
    {
        u32 strl = Str.length();
        Write32(strl);
        char *ch = (char*)memalign(0x1000, strl + 1);
        memset(ch, '\0', strl + 1);
        strcpy(ch, Str.c_str());
        return Write(ch, strl + 1);
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
