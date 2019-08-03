#include <usb/usb_Communications.hpp>
#include <malloc.h>

namespace usb
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

    bool Read8(u8 &Out, UsbCallbackFn Callback)
    {
        u8 num = 0;
        bool ok = (Read(&num, sizeof(u8), Callback) > 0);
        Out = num;
        return ok;
    }

    bool Read32(u32 &Out, UsbCallbackFn Callback)
    {
        u32 num = 0;
        bool ok = (Read(&num, sizeof(u32), Callback) > 0);
        Out = num;
        return ok;
    }

    bool Read64(u64 &Out, UsbCallbackFn Callback)
    {
        u64 num = 0;
        bool ok = (Read(&num, sizeof(u64), Callback) > 0);
        Out = num;
        return ok;
    }

    bool ReadString(std::string &Out, UsbCallbackFn Callback)
    {
        u32 strlen = 0;
        if(!Read32(strlen, Callback)) return false;

        char *raw = new char[strlen + 1]();
        if(Read(raw, strlen, Callback) == 0) return false;
        Out = std::string(raw);

        return true;
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
        char *ch = new char[strl + 1]();
        strcpy(ch, Str.c_str());
        bool ok = Write(ch, strl + 1);
        delete[] ch;
        return ok;
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
