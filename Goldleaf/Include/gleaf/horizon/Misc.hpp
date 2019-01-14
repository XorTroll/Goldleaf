
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <ctime>
#include <cstdio>

namespace gleaf::horizon
{
    struct FwVersion
    {
        u8 Major;
        u8 Minor;
        u8 Micro;
        std::string DisplayName;
        std::string ToString();
    };

    u32 GetBatteryLevel();
    bool IsCharging();
    bool ExportQlaunchRomFs();
    std::string GetCurrentTime();
    FwVersion GetFwVersion();
    std::string FormatHex(u32 Number);
    std::string DoubleToString(double Number);
    u64 GetSdCardFreeSpaceForInstalls();
    u64 GetNANDFreeSpaceForInstalls();
}