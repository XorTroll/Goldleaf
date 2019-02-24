
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
        "Content manager",
        "Location resolver",
        "Process manager",
        "Service manager",
        "NS",
        "ETicket",
        "PSM",
        "Board power control",
        "SPL",
        "Settings",
        "USB",
        "FS",
        "Goldleaf",
        "C (errno)",
    };

    static const set::Dictionary English =
    {
        Language::English,
        {
            "Unknown or undocumented error",
            "Invalid path (might be another filesystem-related error)",
            "Invalid NCA magic (missing sigpatches?) (firmware too low?)",
            "Invalid PFS0 block hash (invalid NSP?)",

            // Other errors
            "Standard C error (from errno value)",
            "Not enough size available",

        }
    };

    static const u32 ErrorModule = 356;
    static const u32 ErrnoErrorModule = 358;

    Result Make(ErrorDescription Description);
    Result MakeErrno(int Val);
    set::Dictionary GetDictionary();
    Error DetermineError(Result OSError);
}