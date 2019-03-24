
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/usb/Commands.hpp>

namespace gleaf::usb
{
    inline bool CallbackStub()
    {
        return false;
    }

    size_t Read(void *Out, size_t Size, UsbCallbackFn Callback = CallbackStub);
    size_t Write(const void *Buffer, size_t Size);
    Command ReadCommand(UsbCallbackFn Callback = CallbackStub);
    u32 Read32(UsbCallbackFn Callback = CallbackStub);
    u64 Read64(UsbCallbackFn Callback = CallbackStub);
    std::string ReadString(u32 Length, UsbCallbackFn Callback = CallbackStub);
    bool WriteCommand(CommandId Id);
    bool Write32(u32 Data);
    u32 GetState();
    bool IsStatePlugged();
    bool IsStateNotReady();
}