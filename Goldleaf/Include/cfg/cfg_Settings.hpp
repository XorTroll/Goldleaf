
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

#pragma once
#include <Types.hpp>

namespace cfg {

    struct WebBookmark {
        std::string name;
        std::string url;
    };

    #define CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(type, name) \
    type name; \
    bool has_##name;

    struct Settings {
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(Language, custom_lang)
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(std::string, external_romfs)
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(pu::ui::Color, scrollbar_color)
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(pu::ui::Color, progressbar_color)
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(ColorScheme, custom_scheme)
        CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(u32, menu_item_size)
        bool ignore_required_fw_ver;
        std::vector<WebBookmark> bookmarks;

        void Save();
        std::string PathForResource(const std::string &res_path);
        JSON ReadJSONResource(const std::string &res_path);
        void ApplyScrollBarColor(pu::ui::elm::Menu::Ref &menu);
        void ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &p_bar);
    };

    Settings ProcessSettings();
    bool Exists();

}