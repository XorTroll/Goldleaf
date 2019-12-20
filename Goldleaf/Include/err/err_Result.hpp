
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
#include <cfg/cfg_Strings.hpp>
#include <cerrno>

namespace err
{
    #define R_DEFINE(mod, name, id) static constexpr Result Result##name = MAKERESULT(module::mod, id);

    namespace result
    {
        namespace module
        {
            static constexpr u32 Goldleaf = 356;
            static constexpr u32 Errno = 357;
        }

        R_DEFINE(Goldleaf, NotEnoughSize, 1)
        R_DEFINE(Goldleaf, MetaNotFound, 2)
        R_DEFINE(Goldleaf, CNMTNotFound, 3)
        R_DEFINE(Goldleaf, TitleAlreadyInstalled, 4)
        R_DEFINE(Goldleaf, EntryAlreadyPresent, 5)
        R_DEFINE(Goldleaf, CouldNotLocateTitleContents, 6)
        R_DEFINE(Goldleaf, CouldNotBuildNSP, 7)
        R_DEFINE(Goldleaf, KeyGenMismatch, 8)
        R_DEFINE(Goldleaf, InvalidNSP, 9)

        static inline Result MakeErrnoResult()
        {
            return MAKERESULT(module::Errno, (u32)errno);
        }
    }

    String GetModuleName(u32 module_id);
    String GetResultDescription(Result rc);
}