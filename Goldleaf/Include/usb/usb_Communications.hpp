
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

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