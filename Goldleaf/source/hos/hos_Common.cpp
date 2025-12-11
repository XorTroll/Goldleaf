
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <fs/fs_FileSystem.hpp>
#include <upd/upd_Update.hpp>

namespace hos {

    namespace {

        u8 g_SystemKeyGeneration = 0;
        bool g_SystemKeyGenerationRead = false;

        bool g_ExitLocked = false;

        // NCA KeyGeneration seems to be pkg1::KeyGeneration + 1
        // https://github.com/Atmosphere-NX/Atmosphere/blob/master/libraries/libexosphere/include/exosphere/pkg1/pkg1_key_generation.hpp#L21

        enum Package1KeyGeneration : u8 {
            Package1KeyGeneration_1_0_0  = 0x00,
            Package1KeyGeneration_3_0_0  = 0x01,
            Package1KeyGeneration_3_0_1  = 0x02,
            Package1KeyGeneration_4_0_0  = 0x03,
            Package1KeyGeneration_5_0_0  = 0x04,
            Package1KeyGeneration_6_0_0  = 0x05,
            Package1KeyGeneration_6_2_0  = 0x06,
            Package1KeyGeneration_7_0_0  = 0x07,
            Package1KeyGeneration_8_1_0  = 0x08,
            Package1KeyGeneration_9_0_0  = 0x09,
            Package1KeyGeneration_9_1_0  = 0x0A,
            Package1KeyGeneration_12_1_0 = 0x0B,
            Package1KeyGeneration_13_0_0 = 0x0C,
            Package1KeyGeneration_14_0_0 = 0x0D,
            Package1KeyGeneration_15_0_0 = 0x0E,
            Package1KeyGeneration_16_0_0 = 0x0F,
            Package1KeyGeneration_17_0_0 = 0x10,
            Package1KeyGeneration_18_0_0 = 0x11,
            Package1KeyGeneration_19_0_0 = 0x12,
            Package1KeyGeneration_20_0_0 = 0x13,
            Package1KeyGeneration_21_0_0 = 0x14,
        };

        u8 GetSystemPackage1KeyGeneration() {
            if(hosversionAtLeast(21,0,0)) {
                return Package1KeyGeneration_21_0_0;
            }
            else if(hosversionAtLeast(20,0,0)) {
                return Package1KeyGeneration_20_0_0;
            }
            else if(hosversionAtLeast(19,0,0)) {
                return Package1KeyGeneration_19_0_0;
            }
            else if(hosversionAtLeast(18,0,0)) {
                return Package1KeyGeneration_18_0_0;
            }
            else if(hosversionAtLeast(17,0,0)) {
                return Package1KeyGeneration_17_0_0;
            }
            else if(hosversionAtLeast(16,0,0)) {
                return Package1KeyGeneration_16_0_0;
            }
            else if(hosversionAtLeast(15,0,0)) {
                return Package1KeyGeneration_15_0_0;
            }
            else if(hosversionAtLeast(14,0,0)) {
                return Package1KeyGeneration_14_0_0;
            }
            else if(hosversionAtLeast(13,0,0)) {
                return Package1KeyGeneration_13_0_0;
            }
            else if(hosversionAtLeast(12,1,0)) {
                return Package1KeyGeneration_12_1_0;
            }
            else if(hosversionAtLeast(9,1,0)) {
                return Package1KeyGeneration_9_1_0;
            }
            else if(hosversionAtLeast(9,0,0)) {
                return Package1KeyGeneration_9_0_0;
            }
            else if(hosversionAtLeast(8,1,0)) {
                return Package1KeyGeneration_8_1_0;
            }
            else if(hosversionAtLeast(7,0,0)) {
                return Package1KeyGeneration_7_0_0;
            }
            else if(hosversionAtLeast(6,2,0)) {
                return Package1KeyGeneration_6_2_0;
            }
            else if(hosversionAtLeast(6,0,0)) {
                return Package1KeyGeneration_6_0_0;
            }
            else if(hosversionAtLeast(5,0,0)) {
                return Package1KeyGeneration_5_0_0;
            }
            else if(hosversionAtLeast(4,0,0)) {
                return Package1KeyGeneration_4_0_0;
            }
            else if(hosversionAtLeast(3,0,1)) {
                return Package1KeyGeneration_3_0_1;
            }
            else if(hosversionAtLeast(3,0,0)) {
                return Package1KeyGeneration_3_0_0;
            }
            else {
                return Package1KeyGeneration_1_0_0;
            }
        }

        inline u8 GetKeyGenerationFromPackage1KeyGeneration(const u8 pkg1_key_gen) {
            return pkg1_key_gen + 1;
        }

        inline u8 GetPackage1KeyGenerationFromKeyGeneration(const u8 key_gen) {
            return key_gen - 1;
        }

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

    NfpDate GetCurrentDate() {
        const auto posix_time = time(nullptr);
        const auto local_time = localtime(&posix_time);

        NfpDate out_date = {};
        out_date.year = static_cast<u16>(local_time->tm_year + 1900);
        out_date.month = static_cast<u8>(local_time->tm_mon + 1);
        out_date.day = static_cast<u8>(local_time->tm_mday);
        return out_date;
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

    std::string FormatApplicationId(const u64 app_id) {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << app_id;
        return strm.str();
    }

    std::string ContentIdAsString(const NcmContentId &cnt_id) {
        char cnt_id_str[FS_MAX_PATH] = {};
        const auto lower = __bswap64(*(u64*)cnt_id.c);
        const auto upper = __bswap64(*(u64*)(cnt_id.c + 0x8));
        snprintf(cnt_id_str, sizeof(cnt_id_str), "%016lx%016lx", lower, upper);
        return cnt_id_str;
    }

    NcmContentId StringAsContentId(const std::string &cnt_id_str) {
        NcmContentId cnt_id = {};
        char lower[0x20] = {0};
        char upper[0x20] = {0};
        memcpy(lower, cnt_id_str.c_str(), 0x10);
        memcpy(upper, cnt_id_str.c_str() + 0x10, 0x10);
        *(u64*)cnt_id.c = __bswap64(strtoul(lower, nullptr, 0x10));
        *(u64*)(cnt_id.c + 0x8) = __bswap64(strtoul(upper, nullptr, 0x10));
        return cnt_id;
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

    u8 GetSystemKeyGeneration() {
        if(!g_SystemKeyGenerationRead) {
            g_SystemKeyGeneration = GetKeyGenerationFromPackage1KeyGeneration(GetSystemPackage1KeyGeneration());
            g_SystemKeyGenerationRead = true;
        }

        return g_SystemKeyGeneration;
    }

    std::string GetKeyGenerationRange(const u8 key_gen) {
        const auto pkg1_key_gen = GetPackage1KeyGenerationFromKeyGeneration(key_gen);
        switch(pkg1_key_gen) {
            case Package1KeyGeneration_1_0_0: {
                return "1.0.0 - 2.3.0";
            }
            case Package1KeyGeneration_3_0_0: {
                return "3.0.0";
            }
            case Package1KeyGeneration_3_0_1: {
                return "3.0.1 - 3.0.2";
            }
            case Package1KeyGeneration_4_0_0: {
                return "4.0.0 - 4.1.0";
            }
            case Package1KeyGeneration_5_0_0: {
                return "5.0.0 - 5.1.0";
            }
            case Package1KeyGeneration_6_0_0: {
                return "6.0.0 - 6.1.0";
            }
            case Package1KeyGeneration_6_2_0: {
                return "6.2.0";
            }
            case Package1KeyGeneration_7_0_0: {
                return "7.0.0 - 8.0.1";
            }
            case Package1KeyGeneration_8_1_0: {
                return "8.1.0 - 8.1.1";
            }
            case Package1KeyGeneration_9_0_0: {
                return "9.0.0 - 9.0.1";
            }
            case Package1KeyGeneration_9_1_0: {
                return "9.1.0 - 12.0.3";
            }
            case Package1KeyGeneration_12_1_0: {
                return "12.1.0";
            }
            case Package1KeyGeneration_13_0_0: {
                return "13.0.0 - 13.2.1";
            }
            case Package1KeyGeneration_14_0_0: {
                return "14.0.0 - 14.1.2";
            }
            case Package1KeyGeneration_15_0_0: {
                return "15.0.0 - 15.0.1";
            }
            case Package1KeyGeneration_16_0_0: {
                return "16.0.0 - 16.0.3";
            }
            case Package1KeyGeneration_17_0_0: {
                return "17.0.0 - 17.0.1";
            }
            case Package1KeyGeneration_18_0_0: {
                return "18.0.0 - 18.1.0";
            }
            case Package1KeyGeneration_19_0_0: {
                return "19.0.0 - 19.0.1";
            }
            case Package1KeyGeneration_20_0_0: {
                return "20.0.0 -";
            }
            default: {
                return "<unknown>";
            }
        }
    }

    Result PerformShutdown(const bool do_reboot) {
        GLEAF_RC_TRY(spsmInitialize());
        GLEAF_RC_TRY(spsmShutdown(do_reboot));
        spsmExit();

        return rc::ResultSuccess;
    }

}
