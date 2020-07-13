
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

#include <hos/hos_Common.hpp>
#include <hos/hos_Titles.hpp>
#include <fs/fs_FileSystem.hpp>
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
        auto charger = ChargerType_None;
        psmGetChargerType(&charger);
        return charger > ChargerType_None;
    }

    std::string GetCurrentTime()
    {
        time_t timet = time(nullptr);
        struct tm *times = localtime((const time_t*)&timet);
        int h = times->tm_hour;
        int min = times->tm_min;
        int s = times->tm_sec;
        char timestr[0x10] = {0};
        sprintf(timestr, "%02d:%02d:%02d", h, min, s);
        return timestr;
    }

    std::string FormatHex128(AccountUid Number)
    {
        auto ptr = reinterpret_cast<u8*>(Number.uid);
        std::stringstream strm;
        strm << std::hex << std::uppercase;
        for(u32 i = 0; i < 16; i++) strm << (u32)ptr[i];
        return strm.str();
    }

    std::string DoubleToString(double Number)
    {
        std::stringstream strm;
        strm << Number;
        return strm.str();
    }

    std::string FormatResult(Result rc)
    {
        char serr[0x10] = {0};
        sprintf(serr, "%04d-%04d", 2000 + R_MODULE(rc), R_DESCRIPTION(rc));
        return serr;
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

    u8 ComputeSystemKeyGeneration()
    {
        FsStorage boot0;
        auto rc = fsOpenBisStorage(&boot0, FsBisPartitionId_BootPartition1Root);
        if(R_SUCCEEDED(rc))
        {
            u32 keygen_ver = 0;
            fsStorageRead(&boot0, 0x2330, &keygen_ver, sizeof(u32));
            fsStorageClose(&boot0);
            return (u8)keygen_ver;
        }
        return 0;
    }

    Result PerformShutdown(bool do_reboot)
    {
        R_TRY(spsmInitialize());
        R_TRY(spsmShutdown(do_reboot));
        spsmExit();

        return 0;
    }
}