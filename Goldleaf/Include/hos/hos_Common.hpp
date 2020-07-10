
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
    std::string FormatResult(Result rc);
    std::string FormatTime(u64 Seconds);

    void LockAutoSleep();
    void UnlockAutoSleep();

    u8 ComputeSystemKeyGeneration();

    Result PerformShutdown(bool do_reboot);

    inline Result PowerOff()
    {
        return PerformShutdown(false);
    }
    
    inline Result Reboot()
    {
        return PerformShutdown(true);
    }
}