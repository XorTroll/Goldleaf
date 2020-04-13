
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#define ERR_RC_UNLESS(expr, rc) ({ \
    if(!(expr)) { \
        return rc; \
    } \
})

#define ERR_RC_TRY(rc) ({ \
    logging::LogFmt("Rc try - %s", #rc); \
    const auto _tmp_rc = (rc); \
    if(R_FAILED((_tmp_rc))) { \
        return _tmp_rc; \
    } \
})

namespace err
{

    namespace result
    {
        static constexpr Result ResultSuccess = 0;

        namespace module
        {
            static constexpr u32 Goldleaf = 356;
            static constexpr u32 Errno = 357;
        }

        #define _ERR_RC_DEFINE(mod, name, id) static constexpr Result Result##name = MAKERESULT(module::mod, id);

        _ERR_RC_DEFINE(Goldleaf, NotEnoughSize, 1)
        _ERR_RC_DEFINE(Goldleaf, MetaNotFound, 2)
        _ERR_RC_DEFINE(Goldleaf, CNMTNotFound, 3)
        _ERR_RC_DEFINE(Goldleaf, TitleAlreadyInstalled, 4)
        _ERR_RC_DEFINE(Goldleaf, EntryAlreadyPresent, 5)
        _ERR_RC_DEFINE(Goldleaf, CouldNotLocateTitleContents, 6)
        _ERR_RC_DEFINE(Goldleaf, CouldNotBuildNSP, 7)
        _ERR_RC_DEFINE(Goldleaf, KeyGenMismatch, 8)
        _ERR_RC_DEFINE(Goldleaf, InvalidNSP, 9)

        #undef _ERR_RC_DEFINE

        static inline Result MakeErrnoResult()
        {
            return MAKERESULT(module::Errno, (u32)errno);
        }
    }

    String GetModuleName(u32 module_id);
    String GetResultDescription(Result rc);
}