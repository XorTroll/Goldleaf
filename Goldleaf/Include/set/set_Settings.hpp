
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
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
        void ApplyScrollBarColor(pu::ui::elm::Menu *Menu);
        void ApplyProgressBarColor(pu::ui::elm::ProgressBar *PBar);
    };

    static const std::string SettingsFile = "sdmc:/" + GoldleafDir + "/settings.json";

    Settings ProcessSettings();
    bool Exists();
}