
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <set/set_Strings.hpp>

namespace err
{
    enum class ErrorDescription
    {
        NotEnoughSize = 1,
        MetaNotFound,
        CNMTNotFound,
        TitleAlreadyInstalled,
        BadGLUCCommand,
        FileDirectoryAlreadyPresent,
        CouldNotLocateTitleContents,
        CouldNotBuildNSP,
    };

    struct Error
    {
        Result OSError;
        pu::String Module;
        pu::String Description;
        u32 DescIndex;
    };

    static const std::vector<pu::String> Modules =
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