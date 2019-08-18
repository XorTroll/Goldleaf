
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <Types.hpp>
#include <usb/usb_CommsBase.hpp>

namespace usb
{
    inline bool CallbackStub()
    {
        return false;
    }
    
    size_t ReadSimple(void *Out, size_t Size, comms::CallbackFunction Callback = CallbackStub);
    size_t WriteSimple(const void *Buffer, size_t Size);
    size_t Read(void *Out, size_t Size, comms::CallbackFunction Callback = CallbackStub);
    size_t Write(const void *Buffer, size_t Size);
    bool Read8(u8 &Out, comms::CallbackFunction Callback = CallbackStub);
    bool Read32(u32 &Out, comms::CallbackFunction Callback = CallbackStub);
    bool Read64(u64 &Out, comms::CallbackFunction Callback = CallbackStub);
    bool ReadString(pu::String &Out, comms::CallbackFunction Callback = CallbackStub);
    bool Write8(u8 Data);
    bool Write32(u32 Data);
    bool Write64(u64 Data);
    bool WriteString(pu::String Str);
    u32 GetState();
    bool IsStatePlugged();
    bool IsStateNotReady();
}