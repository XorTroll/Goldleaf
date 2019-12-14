
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
#include <switch.h>
#include <string>
#include <ctime>
#include <functional>
#include <cstdio>
#include <sstream>
#include <Types.hpp>

namespace hos
{
    u32 GetBatteryLevel();
    bool IsCharging();
    std::string GetCurrentTime();
    
    template<typename N>
    inline std::string FormatHex(N Number)
    {
        std::stringstream strm;
        strm << "0x" << std::hex << std::uppercase << Number;
        return strm.str();
    }

    std::string FormatHex128(AccountUid Number);
    std::string DoubleToString(double Number);
    std::string FormatTime(u64 Seconds);
    u64 GetSdCardFreeSpaceForInstalls();
    u64 GetNANDFreeSpaceForInstalls();
    void IRAMWrite(void *Data, uintptr_t IRAMAddress, size_t Size);
    void IRAMClear();
    void PayloadProcess(String Path);
    void LockAutoSleep();
    void UnlockAutoSleep();
    u8 ComputeSystemKeyGeneration();

    static constexpr size_t MaxPayloadSize = 0x2F000;
    static constexpr size_t IRAMPayloadBaseAddress = 0x40010000;
}