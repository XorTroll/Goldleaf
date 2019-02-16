
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <iostream>
#include <switch.h>
#include <unistd.h>
#include <json.hpp>
#include <pu/Plutonium>

namespace gleaf
{
    enum class RunMode
    {
        Unknown,
        NRO,
        Title,
        Qlaunch,
    };

    enum class Storage
    {
        GameCart = 2,
        NANDSystem,
        NANDUser,
        SdCard,
    };

    enum class InstallerError
    {
        Success,
        BadNSP,
        NSPOpen,
        BadCNMTNCA,
        CNMTMCAOpen,
        BadCNMT,
        CNMTOpen,
        BadControlNCA,
        MetaDatabaseOpen,
        MetaDatabaseSet,
        MetaDatabaseCommit,
        ContentMetaCount,
        ContentMetaList,
        RecordPush,
        InstallBadNCA,
        TitleFound,
    };

    enum class Language
    {
        English,
        Spanish,
        German,
        French,
    };

    struct InstallerResult
    {
        Result Error;
        InstallerError Type;

        bool IsSuccess();
    };

    struct ColorScheme
    {
        pu::draw::Color Background;
        pu::draw::Color Base;
        pu::draw::Color BaseFocus;
        pu::draw::Color Text;
    };
}
