
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

namespace hos {

    namespace {

        u8 g_SystemKeyGeneration = 0;
        bool g_SystemKeyGenerationRead = false;

        bool g_ExitLocked = false;

    }

    u32 GetBatteryLevel() {
        u32 bat = 0;
        psmGetBatteryChargePercentage(&bat);
        return bat;
    }

    bool IsCharging() {
        auto charger = PsmChargerType_Unconnected;
        psmGetChargerType(&charger);
        return charger != PsmChargerType_Unconnected;
    }

    std::string GetCurrentTime(const bool use_12h_time) {
        const auto posix_time = time(nullptr);
        const auto local_time = localtime(&posix_time);

        char time_str[0x20] = {};
        if(use_12h_time) {
            auto hour = local_time->tm_hour;
            if(hour > 12) {
                hour -= 12;
            }
            else if(hour == 0) {
                hour = 12;
            }

            const auto ampm_str = (local_time->tm_hour >= 12) ? "PM" : "AM";
            snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d %s", hour, local_time->tm_min, local_time->tm_sec, ampm_str);
        }
        else {
            snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
        }

        return time_str;
    }

    std::string FormatHex128(const AccountUid user_id) {
        auto ptr = reinterpret_cast<const u8*>(user_id.uid);
        std::stringstream strm;
        strm << std::hex << std::uppercase;
        for(u32 i = 0; i < sizeof(AccountUid); i++) {
            strm << static_cast<u32>(ptr[i]);
        }
        return strm.str();
    }

    std::string FormatResult(const Result rc) {
        char res_err[0x20] = {};
        snprintf(res_err, sizeof(res_err), "%04d-%04d", 2000 + R_MODULE(rc), R_DESCRIPTION(rc));
        return res_err;
    }

    std::string FormatTime(const u64 sec) {
        u64 f_day = 0;
        u64 f_hour = 0;
        u64 f_min = 0;
        u64 f_sec = sec;

        if(f_sec > 60) {
            f_min = f_sec / 60;
            f_sec = f_sec % 60;
            if(f_min > 60) {
                f_hour = f_min / 60;
                f_min = f_min % 60;
                if(f_hour > 24) {
                    f_day = f_hour / 24;
                    f_hour = f_hour % 24;
                }
            }
        }

        std::string time_fmt;
        if(f_day > 0) {
            time_fmt += " " + std::to_string(f_day) + " d";
        }
        if(f_hour > 0) {
            time_fmt += " " + std::to_string(f_hour) + " h";
        }
        if(f_min > 0) {
            time_fmt += " " + std::to_string(f_min) + " min";
        }
        if(f_sec > 0) {
            time_fmt += " " + std::to_string(f_sec) + " s";
        }
        if(!time_fmt.empty()) {
            time_fmt.erase(time_fmt.begin());
        }
        return time_fmt;
    }

    void LockExit() {
        if(GetLaunchMode() == LaunchMode::Application) {
            appletBeginBlockingHomeButton(0);
        }
        appletSetMediaPlaybackState(true);

        g_ExitLocked = true;
    }

    void UnlockExit() {
        if(g_ExitLocked) {
            appletSetMediaPlaybackState(false);
            if(GetLaunchMode() == LaunchMode::Application) {
                appletEndBlockingHomeButton();
            }
            
            g_ExitLocked = false;
        }
    }

    bool IsExitLocked() {
        return g_ExitLocked;
    }

    u8 ReadSystemKeyGeneration() {
        if(!g_SystemKeyGenerationRead) {
            FsStorage boot0;
            if(R_SUCCEEDED(fsOpenBisStorage(&boot0, FsBisPartitionId_BootPartition1Root))) {
                u32 keygen_ver = 0;
                fsStorageRead(&boot0, 0x2330, &keygen_ver, sizeof(u32));
                fsStorageClose(&boot0);
                g_SystemKeyGeneration = static_cast<u8>(keygen_ver);
                g_SystemKeyGenerationRead = true;
            }
        }

        return g_SystemKeyGeneration;
    }

    Result PerformShutdown(const bool do_reboot) {
        GLEAF_RC_TRY(spsmInitialize());
        GLEAF_RC_TRY(spsmShutdown(do_reboot));
        spsmExit();

        return 0;
    }

}