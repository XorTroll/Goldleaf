
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

#include <cfg/cfg_Strings.hpp>
#include <fs/fs_FileSystem.hpp>

extern cfg::Settings global_settings;

namespace cfg
{
    String Strings::GetString(u32 idx)
    {
        if(idx >= this->json.size()) return "???";
        return this->json[idx].get<std::string>();
    }

    namespace strings
    {
        Strings Main;
        Strings Results;
        Strings Modules;
    }

    #define CFG_STRINGS_PROCESS(strs, json_name) { \
        strings::strs.language = global_settings.custom_lang; \
        std::ifstream ifs(global_settings.PathForResource(std::string("/Strings/") + #strs + "/" + json_name)); \
        if(ifs.good()) \
        { \
            try { strings::strs.json = JSON::parse(ifs); } catch(std::exception&) {} \
            ifs.close(); \
        } \
    }

    void LoadStrings()
    {
        auto strjson = LanguageToString(global_settings.custom_lang) + ".json";

        CFG_STRINGS_PROCESS(Main, strjson)
        CFG_STRINGS_PROCESS(Results, strjson)
        CFG_STRINGS_PROCESS(Modules, strjson)
    }
}
