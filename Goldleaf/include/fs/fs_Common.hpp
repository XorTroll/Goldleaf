
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

    enum class Partition {
        PRODINFOF,
        NANDSafe,
        NANDUser = 4,
        NANDSystem = 3,
        SdCard = 5,
    };

    inline void CreateConcatenationFile(const std::string &path) {
        fsdevCreateFile(path.c_str(), 0, FsCreateOption_BigFile);
    }

    using CopyFileCallback = std::function<void(const size_t, const size_t)>;
    using CopyDirectoryCallback = std::function<void(const size_t, const size_t, const std::string&, const size_t, const size_t)>;
    
    void CopyFileProgress(const std::string &path, const std::string &new_path, CopyFileCallback cb_fn);
    void CopyDirectoryProgress(const std::string &dir, const std::string &new_dir, CopyDirectoryCallback cb_fn);
    
    inline std::string GetBaseName(const std::string &path) {
        return path.substr(path.find_last_of("/") + 1);
    }

    inline std::string GetBaseDirectory(const std::string &path) {
        return path.substr(0, path.find_last_of("/"));
    }

    inline std::string GetFileName(const std::string &path) {
        return path.substr(0, path.find_last_of("."));
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

    inline Result MountTitleSaveData(const u64 app_id, const AccountUid user_id, FsFileSystem &out_fs) {
        const FsSaveDataAttribute attr = {
            .application_id = app_id,
            .uid = user_id,
            .save_data_type = FsSaveDataType_Account
        };

        return fsOpenSaveDataFileSystem(std::addressof(out_fs), FsSaveDataSpaceId_User, &attr);
    }

    constexpr size_t DefaultWorkBufferSize = 8_MB;
    constexpr std::align_val_t WorkBufferAlign = std::align_val_t(0x1000);

    // Note: buffers used in FS operations must be allocated this way (for instance, USB requires them to be aligned)

    inline u8 *AllocateWorkBuffer(const size_t size = DefaultWorkBufferSize) {
        return new (WorkBufferAlign) u8[size]();
    }

    inline void DeleteWorkBuffer(u8 *work_buf) {
        operator delete[](work_buf, WorkBufferAlign);
    }

}