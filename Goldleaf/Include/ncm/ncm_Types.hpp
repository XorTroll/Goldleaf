
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
#include <Types.hpp>

namespace ncm {

    struct ContentMetaHeader {
        u64 app_id;
        u32 title_version;
        u8 type;
        u8 pad;
        u16 extended_header_size;
        u16 content_count;
        u16 content_meta_count;
        u8 attributes;
        u8 pad1[0x3];
        u32 required_download_system_version;
        u8 pad2[0x4];
    } PACKED;

}