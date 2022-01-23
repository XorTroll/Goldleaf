
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <cfg/cfg_Strings.hpp>
#include <fs/fs_FileSystem.hpp>

extern cfg::Settings g_Settings;

namespace cfg {

    namespace strings {

        Strings Main;
        Strings Results;
        Strings Modules;
        
    }

    std::string Strings::GetString(u32 idx) {
        if(idx >= this->json.size()) {
            return "???";
        }
        return this->json[idx].get<std::string>();
    }

    #define _CFG_PROCESS_STRINGS(strs, json_name) { \
        strings::strs.language = g_Settings.custom_lang; \
        strings::strs.json = g_Settings.ReadJSONResource(std::string("/Strings/") + #strs + "/" + json_name); \
    }

    void LoadStrings() {
        const auto &str_json = LanguageToString(g_Settings.custom_lang) + ".json";

        _CFG_PROCESS_STRINGS(Main, str_json)
        _CFG_PROCESS_STRINGS(Results, str_json)
        _CFG_PROCESS_STRINGS(Modules, str_json)
    }
}
