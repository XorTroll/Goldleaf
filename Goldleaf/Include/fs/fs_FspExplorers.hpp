
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
            bool dispose;
            FsFileSystem fs;
        
        public:
            FspExplorer(String display_name, FsFileSystem file_system);
            FspExplorer(String display_name, std::string mount_name, FsFileSystem file_system);
            ~FspExplorer();
            bool IsOk();
            FsFileSystem *GetFileSystem();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
    };

    class SdCardExplorer final : public FspExplorer {
        public:
            SdCardExplorer();
    };

    class RomFsExplorer final : public FspExplorer {
        public:
            RomFsExplorer();
    };

    class NANDExplorer final : public FspExplorer {
        private:
            Partition part;
        
        public:
            NANDExplorer(Partition part);
            static FsFileSystem MountNANDFileSystem(Partition part);
            static std::string GetNANDPartitionName(Partition part);
            Partition GetPartition();
    };

    class TitleSaveDataExplorer final : public FspExplorer {
        private:
            u64 appid;
            AccountUid uid;

        public:
            TitleSaveDataExplorer(u64 app_id, AccountUid user_id);
            static FsFileSystem MountTitleSaveData(u64 app_id, AccountUid user_id);
            void DoCommit();
            bool Matches(u64 app_id, AccountUid user_id);
    };

}