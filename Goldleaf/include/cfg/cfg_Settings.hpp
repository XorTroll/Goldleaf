
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

namespace cfg {

    struct WebBookmark {
        std::string name;
        std::string url;
    };

    struct Settings {
        // General
        Language lang;
        std::string external_romfs_path;
        bool use_12h_time;
        bool ignore_hidden_files;

        // UI
        ColorScheme light_color_scheme;
        ColorScheme dark_color_scheme;
        u32 menu_item_size;

        // FS
        bool compute_directory_sizes;
        
        // Installs
        bool ignore_required_fw_ver;
        size_t copy_buffer_max_size;
        bool show_deletion_prompt_after_install;

        // Export
        size_t decrypt_buffer_max_size;

        // Web
        std::vector<WebBookmark> bookmarks;

        bool system_is_light;

        inline ColorScheme &GetColorScheme() {
            return this->system_is_light ? this->light_color_scheme : this->dark_color_scheme;
        }

        void Save();
        std::string PathForResource(const std::string &res_path);
        JSON ReadJSONResource(const std::string &res_path);
        Language GetLanguage();
    };

    Settings ProcessSettings();

}
