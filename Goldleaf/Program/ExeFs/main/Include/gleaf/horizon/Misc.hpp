
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <ctime>
#include <functional>
#include <cstdio>

namespace gleaf::horizon
{
    enum class GpioInput
    {
        Power,
        VolumeDown,
        VolumeUp,
    };

    struct FwVersion
    {
        u8 Major;
        u8 Minor;
        u8 Micro;
        std::string DisplayName;
        std::string ToString();
    };

    class Thread
    {
        public:
            Thread(ThreadFunc Callback);
            ~Thread();
            Result Start(void *Args = NULL);
            Result Join();
            Result Pause();
            Result Resume();
        private:
            ThreadFunc tcb;
            ::Thread nth;
    };

    u32 GetBatteryLevel();
    bool IsCharging();
    bool ExportQlaunchRomFs();
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
    void InitializeGpioInputHandling();
    bool IsGpioInputPressed(GpioInput InputType);
    void FinalizeGpioInputHandling();
}