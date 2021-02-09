
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
#include <string>
#include <vector>
#include <functional>
#include <switch.h>
#include <Types.hpp>

namespace fs
{
    enum class Partition
    {
        PRODINFOF,
        NANDSafe,
        NANDUser = 4,
        NANDSystem = 3,
        SdCard = 5,
    };

    using ifstream = std::basic_ifstream<char16_t>;
    using ofstream = std::basic_ofstream<char16_t>;

    constexpr u64 Size4GB = 0x100000000;
    constexpr u64 WorkBufferSize = 0x800000; // 8MB

    void CreateConcatenationFile(String Path);
    
    void CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback);
    void CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback);
    
    inline String GetFileName(String Path)
    {
        return Path.substr(Path.find_last_of("/") + 1);
    }

    inline String GetBaseDirectory(String Path)
    {
        return Path.substr(0, Path.find_last_of("/"));
    }

    inline String GetExtension(String Path)
    {
        return Path.substr(Path.find_last_of(".") + 1);
    }

    inline String GetPathRoot(String Path)
    {
        return Path.substr(0, Path.find_first_of(":"));
    }

    inline String GetPathWithoutRoot(String Path)
    {
        return Path.substr(Path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(Partition Partition);
    u64 GetFreeSpaceForPartition(Partition Partition);
    String FormatSize(u64 Bytes);

    u8 *GetWorkBuffer();
}