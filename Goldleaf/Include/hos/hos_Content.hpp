
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
#include <string>
#include <cstdio>
#include <cstring>
#include <switch.h>
#include <Types.hpp>
#include <ncm/ncm_ContentMeta.hpp>

namespace hos
{
    struct PendingUpdateVersion
    {
        u32 Major;
        u32 Minor;
        u32 Micro;
    };

    pu::String ContentIdAsString(const NcmContentId &ContentId);
    NcmContentId StringAsContentId(pu::String ContentId);

    bool GetPendingUpdateInfo(PendingUpdateVersion *out);
    SetSysFirmwareVersion ConvertPendingUpdateVersion(PendingUpdateVersion ver);
    void CleanPendingUpdate();
}