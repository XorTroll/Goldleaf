
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

#pragma once
#include <base.hpp>

namespace util {

    template<typename N>
    inline std::string FormatHex(const N num) {
        std::stringstream strm;
        strm << "0x" << std::hex << std::uppercase << num;
        return strm.str();
    }

    std::string FormatHex128(const AccountUid user_id);
    std::string FormatResult(const Result rc);
    std::string FormatTime(const u64 sec);
    std::string FormatApplicationId(const u64 app_id);
    std::string FormatContentId(const NcmContentId &cnt_id);
    NcmContentId GetContentId(const std::string &cnt_id_str);

    std::string JoinVector(const std::vector<std::string> &vec, const std::string &delim);

}
