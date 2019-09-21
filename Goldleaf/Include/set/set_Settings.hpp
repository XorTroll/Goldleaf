
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
        std::string Name;
        std::string URL;
    };

    struct Settings
    {
        Language CustomLanguage;
        std::string KeysPath;
        std::string ExternalRomFs;
        ColorScheme CustomScheme;
        u32 MenuItemSize;
        bool HasScrollBar;
        pu::ui::Color ScrollBarColor;
        bool HasProgressBar;
        pu::ui::Color ProgressBarColor;
        bool IgnoreRequiredFirmwareVersion;
        std::vector<WebBookmark> Bookmarks;

        std::string PathForResource(std::string Path);
        void ApplyScrollBarColor(pu::ui::elm::Menu::Ref &Menu);
        void ApplyProgressBarColor(pu::ui::elm::ProgressBar::Ref &PBar);
    };

    static const std::string SettingsFile = "sdmc:/" + GoldleafDir + "/settings.json";

    Settings ProcessSettings();
    bool Exists();
}