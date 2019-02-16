
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
    };

    struct Error
    {
        Result OSError;
        std::string Description;
        u32 DescIndex;
    };

    static const set::Dictionary English =
    {
        Language::English,
        {
            "Unknown or undocumented error",
            "System error (from errno value)",
            "Invalid path (might be another filesystem-related error)",
            "Invalid NCA magic (probably missing sigpatches)",
            "Invalid PFS block hash (invalid NSP?)",

            // Other errors
            "Not enough size available",
        }
    };

    static const u32 ErrorModule = 356;
    static const u32 ErrnoErrorModule = 358;

    Result Make(ErrorDescription Description);
    set::Dictionary GetDictionary();
    Error DetermineError(Result OSError);
}