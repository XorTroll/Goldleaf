
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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
#include <base_Common.hpp>

namespace ncm {

    struct PackagedContentMetaHeader {
        u64 app_id;
        u32 title_version;
        u8 type;
        u8 pad;
        NcmContentMetaHeader header;
        u8 pad1[0x2];
        u32 required_download_system_version;
        u8 pad2[0x4];
    };

    struct DeltaMetaExtendedHeader {
        u64 app_id;
        u32 extended_data_size;
        u8 reserved[4];
    };

}