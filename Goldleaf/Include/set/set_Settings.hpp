
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

#pragma once
#include <Types.hpp>

namespace set
{
    struct WebBookmark
    {
        std::string name;
        std::string url;
    };

    #define SET_OPTIONAL_VALUE(type, name) \
    type name; \
    bool has_##name;

    struct Settings
    {
        SET_OPTIONAL_VALUE(Language, custom_lang)
        SET_OPTIONAL_VALUE(std::string, external_romfs)
        SET_OPTIONAL_VALUE(pu::ui::Color, scrollbar_color)
        SET_OPTIONAL_VALUE(pu::ui::Color, progressbar_color)
        SET_OPTIONAL_VALUE(ColorScheme, custom_scheme)
        SET_OPTIONAL_VALUE(u32, menu_item_size)

        bool ignore_required_fw_ver;
        std::vector<WebBookmark> bookmarks;

        void Save();
        std::string PathForResource(std::string Path);
        void ApplyScrollBarColor(pu::ui::elm::Menu::Ref &Menu);
        void ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &PBar);
    };

    Settings ProcessSettings();
    bool Exists();
}