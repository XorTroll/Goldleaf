
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

#pragma once
#include <base_Common.hpp>

namespace cfg {

    struct WebBookmark {
        std::string name;
        std::string url;
    };

    #define _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(type, name) \
    type name; \
    bool has_##name;

    struct Settings {
        // General
        _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(Language, custom_lang)
        _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(std::string, external_romfs)
        bool use_12h_time;
        bool ignore_hidden_files;
        
        // UI
        _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(ColorScheme, custom_scheme)
        u32 menu_item_size;
        _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(pu::ui::Color, scrollbar_color)
        _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE(pu::ui::Color, progressbar_color)
        
        // Installs
        bool ignore_required_fw_ver;
        size_t copy_buffer_max_size;

        // Export
        size_t decrypt_buffer_max_size;

        // Web
        std::vector<WebBookmark> bookmarks;

        void Save();
        std::string PathForResource(const std::string &res_path);
        JSON ReadJSONResource(const std::string &res_path);
        void ApplyScrollBarColor(pu::ui::elm::Menu::Ref &menu);
        void ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &p_bar);
        Language GetLanguage();
    };

    #undef _CFG_SETTINGS_DEFINE_OPTIONAL_VALUE

    Settings ProcessSettings();

}