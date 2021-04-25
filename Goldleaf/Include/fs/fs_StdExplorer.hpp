
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
#include <fs/fs_Explorer.hpp>

namespace fs {

    class StdExplorer : public Explorer {
        private:
            std::function<void()> commit_fn;
            FILE *r_file_obj;
            FILE *w_file_obj;
        
        public:
            StdExplorer();
            void SetCommitFunction(std::function<void()> fn);
            virtual std::vector<String> GetDirectories(String path) override;
            virtual std::vector<String> GetFiles(String path) override;
            virtual bool Exists(String path) override;
            virtual bool IsFile(String path) override;
            virtual bool IsDirectory(String path) override;
            virtual void CreateFile(String path) override;
            virtual void CreateDirectory(String path) override;
            virtual void RenameFile(String path, String new_name) override;
            virtual void RenameDirectory(String path, String new_name) override;
            virtual void DeleteFile(String path) override;
            virtual void DeleteDirectory(String path) override;
            virtual void StartFileImpl(String path, FileMode mode) override;
            virtual void EndFileImpl(FileMode mode) override;
            virtual u64 ReadFile(String path, u64 offset, u64 size, void *read_buf) override;
            virtual u64 WriteFile(String path, const void *write_buf, u64 size) override;
            virtual u64 GetFileSize(String path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(String path) override;
    };

}