
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
#include <Types.hpp>

namespace nsp
{
    struct PFS0Header
    {
        u32 Magic;
        u32 FileCount;
        u32 StringTableSize;
        u32 Reserved;
    } PACKED;

    struct PFS0FileEntry
    {
        u64 Offset;
        u64 Size;
        u32 StringTableOffset;
        u32 Pad;
    } PACKED;

    struct PFS0File
    {
        PFS0FileEntry Entry;
        String Name;
    };

    static constexpr u32 Magic = 0x30534650;
}