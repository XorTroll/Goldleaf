
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <gleaf/Types.hpp>
#include <gleaf/ncm/Content.hpp>

namespace gleaf::horizon
{
    struct Title
    {
        u64 ApplicationId;
        Storage Location;
    };

    struct ExtendedTitle
    {
        Title Base;
        std::string Name;
        std::string Author;
        std::string Version;
        u8 *Icon;
        NacpStruct NACP;
        void DumpIconAndNACP(std::string IconPath, std::string NACPPath);
    };

    std::vector<Title> GetAllSystemTitles();
    ExtendedTitle LoadExtendedData(Title BaseTitle);
    u64 GetBaseApplicationId(u64 ApplicationId, ncm::ContentMetaType Type);
}