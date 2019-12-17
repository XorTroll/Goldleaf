
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
#include <set/set_Strings.hpp>

namespace err
{
    enum class ErrorDescription
    {
        NotEnoughSize = 1,
        MetaNotFound,
        CNMTNotFound,
        TitleAlreadyInstalled,
        FileDirectoryAlreadyPresent,
        CouldNotLocateTitleContents,
        CouldNotBuildNSP,
        KeyGenMismatch,
        InvalidNSP
    };

    struct Error
    {
        Result OSError;
        String Module;
        String Description;
        u32 DescIndex;
    };

    static const std::vector<String> Modules =
    {
        "(...)",
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