
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

namespace fs {

    // TODO: just use StorageId?
    enum class Partition {
        PRODINFOF,
        NANDSafe,
        NANDUser = 4,
        NANDSystem = 3,
        SdCard = 5,
    };

    constexpr size_t Size4GB = 0x100000000;
    constexpr size_t WorkBufferSize = 0x800000; // 8MB

    void CreateConcatenationFile(const std::string &path);

    using CopyCallback = std::function<void(const double, const double)>;
    
    void CopyFileProgress(const std::string &path, const std::string &new_path, CopyCallback cb_fn);
    void CopyDirectoryProgress(const std::string &dir, const std::string &new_dir, CopyCallback cb_fn);
    
    inline std::string GetFileName(const std::string &path) {
        return path.substr(path.find_last_of("/") + 1);
    }

    inline std::string GetBaseDirectory(const std::string &path) {
        return path.substr(0, path.find_last_of("/"));
    }

    inline std::string GetExtension(const std::string &path) {
        return path.substr(path.find_last_of(".") + 1);
    }

    inline std::string GetPathRoot(const std::string &path) {
        return path.substr(0, path.find_first_of(":"));
    }

    inline std::string GetPathWithoutRoot(const std::string &path) {
        return path.substr(path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(const Partition partition);
    u64 GetFreeSpaceForPartition(const Partition partition);
    std::string FormatSize(const u64 bytes);

    u8 *GetWorkBuffer();

}