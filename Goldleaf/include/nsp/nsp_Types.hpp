
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

namespace nsp {

    struct PFS0Header {
        u32 magic;
        u32 file_count;
        u32 string_table_size;
        u32 reserved;
    };
    static_assert(sizeof(PFS0Header) == 0x10);

    struct PFS0FileEntry {
        u64 offset;
        u64 size;
        u32 string_table_offset;
        u32 pad;
    };
    static_assert(sizeof(PFS0FileEntry) == 0x18);

    struct PFS0File {
        PFS0FileEntry entry;
        String name;
    };

    constexpr u32 Magic = 0x30534650;

}