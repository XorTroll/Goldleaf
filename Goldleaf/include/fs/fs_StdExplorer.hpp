
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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
        public:
            using CommitFunction = std::function<void()>;

        private:
            CommitFunction commit_fn;
            FILE *r_file_obj;
            FILE *w_file_obj;

            inline void DoCommit() {
                if(this->commit_fn) {
                    (this->commit_fn)();
                }
            }
        
        public:
            StdExplorer();
            void SetCommitFunction(CommitFunction fn);
            virtual std::vector<std::string> GetDirectories(const std::string &path) override;
            virtual std::vector<std::string> GetFiles(const std::string &path) override;
            virtual bool Exists(const std::string &path) override;
            virtual bool IsFile(const std::string &path) override;
            virtual bool IsDirectory(const std::string &path) override;
            virtual void CreateFile(const std::string &path) override;
            virtual void CreateDirectory(const std::string &path) override;
            virtual void RenameFile(const std::string &path, const std::string &new_name) override;
            virtual void RenameDirectory(const std::string &path, const std::string &new_name) override;
            virtual void DeleteFile(const std::string &path) override;
            virtual void DeleteDirectory(const std::string &path) override;
            virtual void StartFileImpl(const std::string &path, const FileMode mode) override;
            virtual void EndFileImpl(const FileMode mode) override;
            virtual u64 ReadFile(const std::string &path, u64 offset, u64 size, void *read_buf) override;
            virtual u64 WriteFile(const std::string &path, const void *write_buf, u64 size) override;
            virtual u64 GetFileSize(const std::string &path) override;
            virtual u64 GetTotalSpace() override;
            virtual u64 GetFreeSpace() override;
            virtual void SetArchiveBit(const std::string &path) override;
    };

}