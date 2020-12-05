
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
#include <fs/fs_StdExplorer.hpp>

namespace fs
{
    class FspExplorer : public StdExplorer
    {
        public:
            FspExplorer(String DisplayName, FsFileSystem FileSystem);
            FspExplorer(String DisplayName, std::string mount_name, FsFileSystem FileSystem);
            ~FspExplorer();
            bool IsOk();
            FsFileSystem *GetFileSystem();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            bool dispose;
            FsFileSystem fs;
    };

    class SdCardExplorer final : public FspExplorer
    {
        public:
            SdCardExplorer();
    };

    class NANDExplorer final : public FspExplorer
    {
        public:
            NANDExplorer(Partition Part);
            static FsFileSystem MountNANDFileSystem(Partition part);
            static std::string GetNANDPartitionName(Partition part);
            Partition GetPartition();
        private:
            Partition part;
    };

    class TitleSaveDataExplorer final : public FspExplorer
    {
        public:
            TitleSaveDataExplorer(u64 app_id, AccountUid user_id);
            static FsFileSystem MountTitleSaveData(u64 app_id, AccountUid user_id);
            void DoCommit();
            bool Matches(u64 app_id, AccountUid user_id);
        private:
            u64 appid;
            AccountUid uid;
    };
}