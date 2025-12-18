
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

#include <util/util_String.hpp>

namespace util {

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

    std::string FormatContentId(const NcmContentId &cnt_id) {
        char cnt_id_str[FS_MAX_PATH] = {};
        const auto lower = __bswap64(*(u64*)cnt_id.c);
        const auto upper = __bswap64(*(u64*)(cnt_id.c + 0x8));
        snprintf(cnt_id_str, sizeof(cnt_id_str), "%016lx%016lx", lower, upper);
        return cnt_id_str;
    }

    NcmContentId GetContentId(const std::string &cnt_id_str) {
        NcmContentId cnt_id = {};
        char lower[0x20] = {0};
        char upper[0x20] = {0};
        memcpy(lower, cnt_id_str.c_str(), 0x10);
        memcpy(upper, cnt_id_str.c_str() + 0x10, 0x10);
        *(u64*)cnt_id.c = __bswap64(strtoul(lower, nullptr, 0x10));
        *(u64*)(cnt_id.c + 0x8) = __bswap64(strtoul(upper, nullptr, 0x10));
        return cnt_id;
    }

    std::string JoinVector(const std::vector<std::string> &vec, const std::string &delim) {
        std::string result;
        for(size_t i = 0; i < vec.size(); i++) {
            result += vec[i];
            if(i != vec.size() - 1) {
                result += delim;
            }
        }
        return result;
    }

}
