
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/set.hpp>

namespace gleaf::err
{
    enum class ErrorDescription
    {
        NotEnoughSize = 1,
        MetaNotFound,
        CNMTNotFound,
        TitleAlreadyInstalled,
        BadGLUCCommand,
        FileDirectoryAlreadyPresent,
    };

    struct Error
    {
        Result OSError;
        std::string Module;
        std::string Description;
        u32 DescIndex;
    };

    static const std::vector<std::string> Modules =
    {
        "Applet",
        "Account",
        "NCM",
        "LR",
        "PM",
        "SM",
        "NS",
        "ETicket",
        "PSM",
        "BPC",
        "SPL",
        "Set",
        "USB",
        "FS",
        "Goldleaf",
        "C (errno)",
    };

    static const u32 ErrorModule = 356;
    static const u32 ErrnoErrorModule = 358;

    Result Make(ErrorDescription Description);
    Result MakeErrno(int Val);
    Error DetermineError(Result OSError);
}