
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <base.hpp>

namespace fs {

    enum class Partition {
        PRODINFOF,
        NANDSafe,
        NANDUser,
        NANDSystem,
        SdCard
    };

    inline Partition GetPartitionFromStorageId(const NcmStorageId id) {
        switch(id) {
            case NcmStorageId_SdCard: {
                return Partition::SdCard;
            }
            case NcmStorageId_BuiltInSystem: {
                return Partition::NANDSystem;
            }
            case NcmStorageId_BuiltInUser: {
                return Partition::NANDUser;
            }
            default: {
                GLEAF_ASSERT_FAIL("Invalid StorageId for partition conversion");
            }
        }
    }

    inline void CreateConcatenationFile(const std::string &path) {
        fsdevCreateFile(path.c_str(), 0, FsCreateOption_BigFile);
    }

    using CopyFileStartCallback = std::function<void(const size_t)>;
    using CopyFileProgressCallback = std::function<void(const size_t)>;

    void CopyFileProgress(const std::string &path, const std::string &new_path, CopyFileStartCallback start_cb, CopyFileProgressCallback prog_cb);

    using CopyDirectoryStartCallback = std::function<void(const size_t)>;
    using CopyDirectoryFileStartCallback = std::function<void(const size_t, const std::string&, const std::string&)>;
    using CopyDirectoryFileProgressCallback = std::function<void(const size_t)>;

    void CopyDirectoryProgress(const std::string &dir, const std::string &new_dir, CopyDirectoryStartCallback start_cb, CopyDirectoryFileStartCallback file_start_cb, CopyDirectoryFileProgressCallback file_prog_cb);
    
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

    inline Result MountDeviceSaveData(const u64 app_id, FsFileSystem &out_fs) {
        const FsSaveDataAttribute attr = {
            .application_id = app_id,
            .save_data_type = FsSaveDataType_Device
        };

        return fsOpenSaveDataFileSystem(std::addressof(out_fs), FsSaveDataSpaceId_User, &attr);
    }
    
    inline Result MountAccountSaveData(const u64 app_id, const AccountUid user_id, FsFileSystem &out_fs) {
        const FsSaveDataAttribute attr = {
            .application_id = app_id,
            .uid = user_id,
            .save_data_type = FsSaveDataType_Account
        };

        return fsOpenSaveDataFileSystem(std::addressof(out_fs), FsSaveDataSpaceId_User, &attr);
    }

    constexpr size_t DefaultWorkBufferSize = 8_MB;
    constexpr size_t WorkBufferAlignment = 0x1000;
    constexpr std::align_val_t WorkBufferAlign = std::align_val_t(WorkBufferAlignment);

    // Note: buffers used in FS operations must be allocated this way (for instance, USB requires them to be aligned)

    inline u8 *AllocateWorkBuffer(const size_t size = DefaultWorkBufferSize) {
        return new (WorkBufferAlign) u8[size]();
    }

    inline void DeleteWorkBuffer(u8 *work_buf) {
        operator delete[](work_buf, WorkBufferAlign);
    }

}
