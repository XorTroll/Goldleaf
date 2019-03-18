#include <gleaf/usb/Communications.hpp>
#include <malloc.h>

namespace gleaf::usb
{
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

    Command ReadCommand(UsbCallbackFn Callback)
    {
        Command cmd;
        memset(&cmd, 0, sizeof(cmd));
        u32 magic = 0;
        Read(&magic, sizeof(u32), Callback);
        u32 cmdid = 0;
        Read(&cmdid, sizeof(u32), Callback);
        cmd.Magic = magic;
        cmd.CommandId = cmdid;
        return cmd;
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

    std::string ReadString(u32 Length, UsbCallbackFn Callback)
    {
        char *data = (char*)memalign(0x1000, Length + 1);
        if(Read(data, Length, Callback) == 0) return "";
        data[Length] = '\0';
        std::string str = std::string(data);
        free(data);
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

    bool Write32(u32 Data)
    {
        return (Write(&Data, sizeof(u32)) != 0);
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
