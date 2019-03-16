#include <gleaf/usb/Communications.hpp>
#include <malloc.h>

namespace gleaf::usb
{
    size_t Read(void *Out, size_t Size, UsbCallbackFn LoopCallback)
    {
        size_t read = 0;
        Result rc = usbCommsRead(Out, Size, &read, LoopCallback);
        if(rc != 0) return 0;
        return read;
    }

    size_t Write(const void *Buffer, size_t Size)
    {
        return usbCommsWrite(Buffer, Size);
    }

    Command ReadCommand(UsbCallbackFn Callback)
    {
        Command cmd;
        memset(&cmd, 0, sizeof(cmd));
        Command *data = (Command*)memalign(0x1000, sizeof(Command));
        Read(data, sizeof(Command), Callback);
        memcpy(&cmd, data, sizeof(cmd));
        free(data);
        return cmd;
    }

    u32 Read32(UsbCallbackFn Callback)
    {
        u32 *data = (u32*)memalign(0x1000, sizeof(u32));
        u32 num = 0;
        if(Read(data, sizeof(u32)) != 0) num = *data;
        free(data);
        return num;
    }

    u64 Read64(UsbCallbackFn Callback)
    {
        u64 *data = (u64*)memalign(0x1000, sizeof(u64));
        u64 num = 0;
        if(Read(data, sizeof(u64)) != 0) num = *data;
        free(data);
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
        const Command *cmd = (const Command*)memalign(0x1000, sizeof(Command));
        memcpy((u32*)&cmd->Magic, &GLUC, sizeof(u32));
        u32 cmdid = static_cast<u32>(Id);
        memcpy((u32*)&cmd->CommandId, &cmdid, sizeof(u32));
        bool ok = (Write(cmd, sizeof(Command)) != 0);
        free((Command*)cmd);
        return ok;
    }

    bool Write32(u32 Data)
    {
        const u32 *data = (const u32*)memalign(0x1000, sizeof(u32));
        memcpy((void*)data, &Data, sizeof(u32));
        return (Write(data, sizeof(u32)) != 0);
    }

    size_t WriteFixed(const void *Buffer, size_t Size)
    {
        const u8 *bufptr = (const u8*)Buffer;
        size_t sz = Size;
        size_t tsz = 0;
        while(sz)
        {
            tsz = Write(bufptr, sz);
            if(tsz == 0) return 0;
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
