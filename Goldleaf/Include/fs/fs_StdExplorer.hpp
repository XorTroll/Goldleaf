
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
#include <fs/fs_Explorer.hpp>
#include <functional>

namespace fs
{

    class StdExplorer : public Explorer
    {
        public:
            StdExplorer();
            void SetCommitFunction(std::function<void()> fn);
            virtual std::vector<String> GetDirectories(String Path) override;
            virtual std::vector<String> GetFiles(String Path) override;
            virtual bool Exists(String Path) override;
            virtual bool IsFile(String Path) override;
            virtual bool IsDirectory(String Path) override;
            virtual void CreateFile(String Path) override;
            virtual void CreateDirectory(String Path) override;
            virtual void RenameFile(String Path, String NewName) override;
            virtual void RenameDirectory(String Path, String NewName) override;
            virtual void DeleteFile(String Path) override;
            virtual void DeleteDirectorySingle(String Path) override;
            virtual void StartFile(String path, FileMode mode) override;
            virtual u64 ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out) override;
            virtual u64 WriteFileBlock(String Path, u8 *Data, u64 Size) override;
            virtual void EndFile(FileMode mode) override;
            virtual u64 GetFileSize(String Path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(String Path) override;
        private:
            std::function<void()> commit_fn;
            FILE *r_file_obj;
            FILE *w_file_obj;
    };
}