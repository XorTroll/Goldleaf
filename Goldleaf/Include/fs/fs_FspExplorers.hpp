
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
#include <fs/fs_StdExplorer.hpp>

namespace fs
{
    class SdCardExplorer final : public StdExplorer
    {
        public:
            SdCardExplorer();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
    };

    class NANDExplorer final : public StdExplorer
    {
        public:
            NANDExplorer(Partition Part);
            ~NANDExplorer();
            Partition GetPartition();
            virtual bool ShouldWarnOnWriteAccess() override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            Partition part;
            FsFileSystem fs;
    };

    class FileSystemExplorer final : public StdExplorer
    {
        public:
            FileSystemExplorer(String MountName, String DisplayName, FsFileSystem *FileSystem);
            ~FileSystemExplorer();
            FsFileSystem *GetFileSystem();
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
        private:
            FsFileSystem *fs;
    };
}