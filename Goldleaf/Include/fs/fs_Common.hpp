
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

namespace fs {

    enum class Partition {
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

    void CreateConcatenationFile(String path);
    
    void CopyFileProgress(String path, String new_path, std::function<void(double Done, double Total)> cb_fn);
    void CopyDirectoryProgress(String dir, String new_dir, std::function<void(double Done, double Total)> cb_fn);
    
    inline String GetFileName(String path) {
        return path.substr(path.find_last_of("/") + 1);
    }

    inline String GetBaseDirectory(String path) {
        return path.substr(0, path.find_last_of("/"));
    }

    inline String GetExtension(String path) {
        return path.substr(path.find_last_of(".") + 1);
    }

    inline String GetPathRoot(String path) {
        return path.substr(0, path.find_first_of(":"));
    }

    inline String GetPathWithoutRoot(String path) {
        return path.substr(path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(Partition partition);
    u64 GetFreeSpaceForPartition(Partition partition);
    String FormatSize(u64 bytes);

    u8 *GetWorkBuffer();

}