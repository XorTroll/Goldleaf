
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
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
    enum class Destination
    {
        NAND,
        SdCard,
    };

    enum class Storage
    {
        SdCard,
        NAND,
        GameCart,
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
    };

    struct InstallerResult
    {
        Result Error;
        InstallerError Type;

        bool IsSuccess();
    };
}