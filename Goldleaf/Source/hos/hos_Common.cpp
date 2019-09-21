
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

#include <hos/hos_Common.hpp>
#include <hos/hos_Titles.hpp>
#include <fs/fs_Explorer.hpp>
#include <sstream>

namespace hos
{
    u32 GetBatteryLevel()
    {
        u32 bat = 0;
        psmGetBatteryChargePercentage(&bat);
        return bat;
    }

    bool IsCharging()
    {
        ChargerType charger = ChargerType_None;
        psmGetChargerType(&charger);
        return (charger > ChargerType_None);
    }

    std::string GetCurrentTime()
    {
        time_t timet = time(NULL);
        struct tm *times = localtime((const time_t*)&timet);
        int h = times->tm_hour;
        int min = times->tm_min;
        int s = times->tm_sec;
        char timestr[0x10] = {0};
        sprintf(timestr, "%02d:%02d:%02d", h, min, s);
        return std::string(timestr);
    }

    std::string FormatHex128(u128 Number)
    {
        u8 *ptr = (u8*)&Number;
        std::stringstream strm;
        for(u32 i = 0; i < 16; i++) strm << std::hex << std::uppercase << (int)ptr[i];
        return strm.str();
    }

    std::string DoubleToString(double Number)
    {
        std::stringstream strm;
        strm << Number;
        return strm.str();
    }

    std::string FormatTime(u64 Seconds)
    {
        u64 secs = Seconds;
        std::string base = std::to_string(secs) + "s";
        if(Seconds > 60)
        {
            auto divt = div(Seconds, 60);
            u64 mins = divt.quot;
            secs = divt.rem;
            base = std::to_string(mins) + "min";
            if(secs > 0) base += (" " + std::to_string(secs) + "s");
            if(mins >= 60)
            {
                auto divt2 = div(mins, 60);
                u64 hrs = divt2.quot;
                mins = divt2.rem;
                base = std::to_string(hrs) + "h";
                if(mins > 0) base += (" " + std::to_string(mins) + "min");
                if(secs > 0) base += (" " + std::to_string(secs) + "s");
                if(hrs >= 24)
                {
                    auto divt3 = div(hrs, 24);
                    u64 days = divt3.quot;
                    hrs = divt3.rem;
                    base = std::to_string(days) + "d";
                    if(hrs > 0) base += (" " + std::to_string(hrs) + "h");
                    if(mins > 0) base += (" " + std::to_string(mins) + "min");
                    if(secs > 0) base += (" " + std::to_string(secs) + "s");
                    if(days >= 7)
                    {
                        auto divt4 = div(days, 7);
                        u64 weeks = divt4.quot;
                        days = divt4.rem;
                        base = std::to_string(weeks) + "w";
                        if(days > 0) base += (" " + std::to_string(days) + "d");
                        if(hrs > 0) base += (" " + std::to_string(hrs) + "h");
                        if(mins > 0) base += (" " + std::to_string(mins) + "min");
                        if(secs > 0) base += (" " + std::to_string(secs) + "s");
                        if(weeks >= 52)
                        {
                            auto divt5 = div(weeks, 52);
                            u64 years = divt5.quot;
                            weeks = divt5.rem;
                            base = std::to_string(years) + "y";
                            if(days > 0) base += (" " + std::to_string(weeks) + "w");
                            if(days > 0) base += (" " + std::to_string(days) + "d");
                            if(hrs > 0) base += (" " + std::to_string(hrs) + "h");
                            if(mins > 0) base += (" " + std::to_string(mins) + "min");
                            if(secs > 0) base += (" " + std::to_string(secs) + "s");
                        }
                    }
                }
            }
        }
        return base;
    }

    u64 GetSdCardFreeSpaceForInstalls()
    {
        return fs::GetFreeSpaceForPartition(fs::Partition::SdCard);
    }

    u64 GetNANDFreeSpaceForInstalls()
    {
        return fs::GetFreeSpaceForPartition(fs::Partition::NANDUser);
    }

    void IRAMWrite(void *Data, uintptr_t IRAMAddress, size_t Size)
    {
        u8 *block = new (std::align_val_t(0x1000)) u8[0x1000]();
        memcpy(block, Data, Size);
        SecmonArgs args = {};
        args.X[0] = 0xF0000201;
        args.X[1] = (uintptr_t)block;
        args.X[2] = IRAMAddress;
        args.X[3] = Size;
        args.X[4] = 1;
        svcCallSecureMonitor(&args);
        memcpy(Data, block, Size);
        operator delete[](block, std::align_val_t(0x1000));
    }

    void IRAMClear()
    {
        u8 *block = new (std::align_val_t(0x1000)) u8[0x1000]();
        memset(block, 0xFF, 0x1000);
        for(u32 i = 0; i < MaxPayloadSize; i += 0x1000) IRAMWrite(block, (IRAMPayloadBaseAddress + i), 0x1000);
        operator delete[](block, std::align_val_t(0x1000));
    }

    void PayloadProcess(pu::String Path)
    {
        u8 *block = new (std::align_val_t(0x1000)) u8[MaxPayloadSize]();
        auto fexp = fs::GetExplorerForMountName(fs::GetPathRoot(Path));
        auto size = fexp->GetFileSize(Path);
        if((size == 0) || (size > MaxPayloadSize)) return;
        fexp->ReadFileBlock(Path, 0, size, block);

        IRAMClear();
        for(u32 i = 0; i < MaxPayloadSize; i += 0x1000) IRAMWrite(&block[i], (IRAMPayloadBaseAddress + i), 0x1000);
        operator delete[](block, std::align_val_t(0x1000));

        splSetConfig((SplConfigItem)65001, 2);
    }

    void LockAutoSleep()
    {
        if(GetLaunchMode() == LaunchMode::Application) appletBeginBlockingHomeButton(0);
        appletSetMediaPlaybackState(true);
    }

    void UnlockAutoSleep()
    {
        appletSetMediaPlaybackState(false);
        if(GetLaunchMode() == LaunchMode::Application) appletEndBlockingHomeButton();
    }

    #define VERSION_EXACT(a,b,c) (hosversionGet() == MAKEHOSVERSION(a,b,c))
    #define VERSION_BETWEEN(a,b,c,d,e,f) ((hosversionGet() >= MAKEHOSVERSION(a,b,c)) && (hosversionGet() <= MAKEHOSVERSION(d,e,f)))

    #define MKEY_SET_IF(expression, kgen) if(expression) { masterkey = kgen; }

    u8 ComputeSystemKeyGeneration()
    {
        u8 masterkey = 0;
        
        MKEY_SET_IF(VERSION_BETWEEN(1,0,0,2,3,0), 0)
        MKEY_SET_IF(VERSION_EXACT(3,0,0), 1)
        MKEY_SET_IF(VERSION_BETWEEN(3,0,1,3,0,2), 2)
        MKEY_SET_IF(VERSION_BETWEEN(4,0,0,4,1,0), 3)
        MKEY_SET_IF(VERSION_BETWEEN(5,0,0,5,1,0), 4)
        MKEY_SET_IF(VERSION_BETWEEN(6,0,0,6,1,0), 5)
        MKEY_SET_IF(VERSION_EXACT(6,2,0), 6)
        MKEY_SET_IF(VERSION_BETWEEN(7,0,0,8,0,1), 7)
        MKEY_SET_IF(VERSION_EXACT(8,1,0), 8)
        MKEY_SET_IF(VERSION_EXACT(9,0,0), 9)

        // KeyGen = MasterKey + 1!
        return masterkey + 1;
    }
}