
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <ctime>
#include <functional>
#include <cstdio>

namespace hos
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
    std::string GetCurrentTime();
    FwVersion GetFwVersion();
    std::string FormatHex(u32 Number);
    std::string FormatHex128(u128 Number);
    std::string DoubleToString(double Number);
    u64 GetSdCardFreeSpaceForInstalls();
    u64 GetNANDFreeSpaceForInstalls();
    void IRAMWrite(void *Data, uintptr_t IRAMAddress, size_t Size);
    void IRAMClear();
    void PayloadProcess(std::string Path);
}