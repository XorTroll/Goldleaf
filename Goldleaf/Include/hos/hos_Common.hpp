
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
#include <sstream>
#include <Types.hpp>

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
    
    template<typename N>
    inline std::string FormatHex(N Number)
    {
        std::stringstream strm;
        strm << "0x" << std::hex << std::uppercase << Number;
        return strm.str();
    }

    std::string FormatHex128(u128 Number);
    std::string DoubleToString(double Number);
    std::string FormatTime(u64 Seconds);
    u64 GetSdCardFreeSpaceForInstalls();
    u64 GetNANDFreeSpaceForInstalls();
    void IRAMWrite(void *Data, uintptr_t IRAMAddress, size_t Size);
    void IRAMClear();
    void PayloadProcess(pu::String Path);
    void LockAutoSleep();
    void UnlockAutoSleep();
    u8 ComputeSystemKeyGeneration();

    static constexpr size_t MaxPayloadSize = 0x2F000;
    static constexpr size_t IRAMPayloadBaseAddress = 0x40010000;
}