
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
#include <fs/fs_StdExplorer.hpp>

namespace fs {

    class FspExplorer : public StdExplorer {
        private:
            FsFileSystem fs;
            bool dispose;
        
        public:
            FspExplorer(FsFileSystem fs, const std::string &display_name, const std::string &mount_name = "");
            ~FspExplorer();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
    };

    class SdCardExplorer final : public FspExplorer {
        public:
            SdCardExplorer() : FspExplorer(*fsdevGetDeviceFileSystem("sdmc"), "SdCard", "sdmc") {}
    };

    class RomFsExplorer final : public FspExplorer {
        public:
            RomFsExplorer() : FspExplorer({}, "RomFs", "romfs") {}
    };

    class NANDExplorer final : public FspExplorer {
        private:
            Partition part;
        
        public:
            NANDExplorer(const Partition part);
            static FsFileSystem MountNANDFileSystem(const Partition part);
            static std::string GetNANDPartitionName(const Partition part);
    };

    inline Result MountTitleSaveData(const u64 app_id, const AccountUid user_id, FsFileSystem &out_fs) {
        const FsSaveDataAttribute attr = {
            .application_id = app_id,
            .uid = user_id,
            .save_data_type = FsSaveDataType_Account
        };

        return fsOpenSaveDataFileSystem(std::addressof(out_fs), FsSaveDataSpaceId_User, &attr);
    }

}