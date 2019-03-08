
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/es.hpp>
#include <gleaf/ns.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf::set
{
    struct Settings
    {
        Language CustomLanguage;
        std::string KeysPath;
        bool AllowRemoveSystemTitles;
        bool WarnRemoveUsedTickets;
        std::string RomFsReplacePath;
        ColorScheme CustomScheme;
        u32 BrowserItemSize;

        std::string PathForResource(std::string Path);
    };

    Settings ProcessSettings();
    bool Exists();
}