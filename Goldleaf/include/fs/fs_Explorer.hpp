
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
#include <fs/fs_Common.hpp>

namespace fs {

    enum class FileMode : u32 {
        None,
        Read,
        Write,
        Append
    };

    class Explorer {
        protected:
            std::string disp_name;
            std::string mnt_name;
            std::string cwd;
            bool warn_write;
            FileMode started_file_mode;

        public:
            Explorer() : warn_write(false), started_file_mode(FileMode::None) {}
            virtual ~Explorer() {}

            void SetNames(const std::string &mount_name, const std::string &display_name);
            bool NavigateBack();
            bool NavigateForward(const std::string &path);
            std::vector<std::string> GetContents();
            
            inline std::string GetMountName() {
                return this->mnt_name;
            }

            inline std::string GetCwd() {
                return this->cwd;
            }

            std::string GetPresentableCwd();
        
            inline std::string FullPathFor(const std::string &path) {
                auto new_path = this->cwd;
                if(this->cwd.substr(this->cwd.length() - 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline std::string AbsolutePathFor(const std::string &path) {
                auto new_path = this->mnt_name + ":";
                if(path.substr(0, 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline std::string FullPresentablePathFor(const std::string &path) {
                auto pres_cwd = this->GetPresentableCwd();
                auto new_path = pres_cwd;
                if(pres_cwd.substr(pres_cwd.length() - 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline std::string AbsolutePresentablePathFor(const std::string &path) {
                auto new_path = this->disp_name + ":";
                if(path.substr(0, 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline std::string MakeFull(const std::string &path) {
                return this->IsFullPath(path) ? path : this->FullPathFor(path);
            }

            inline std::string MakeFullPresentable(const std::string &path) {
                return this->IsFullPath(path) ? path : this->FullPresentablePathFor(path);
            }

            inline std::string MakeAbsolute(const std::string &path) {
                return this->IsFullPath(path) ? path : this->AbsolutePathFor(path);
            }

            inline std::string MakeAbsolutePresentable(const std::string &path) {
                return this->IsFullPath(path) ? path : this->AbsolutePresentablePathFor(path);
            }

            inline std::string RemoveMountName(const std::string &path) {
                return path.substr(this->mnt_name.length() + 1);
            }

            inline bool IsFullPath(const std::string &path) {
                return path.find(":/") != std::string::npos;
            }

            void CopyFile(const std::string &path, const std::string &new_path);
            void CopyFileProgress(const std::string &path, const std::string &new_path, CopyFileCallback cb_fn);
            void CopyDirectory(const std::string &dir, const std::string &new_dir);
            void CopyDirectoryProgress(const std::string &dir, const std::string &new_dir, CopyDirectoryCallback cb_fn);
            bool IsFileBinary(const std::string &path);
            std::vector<u8> ReadFile(const std::string &path);
            JSON ReadJSON(const std::string &path);

            inline void WriteJSON(const std::string &path, const JSON &json) {
                const auto json_str = json.dump(4);
                this->WriteFile(path, json_str.c_str(), json_str.length());
            }

            std::vector<std::string> ReadFileLines(const std::string &path, const u32 line_offset, const u32 line_count);
            std::vector<std::string> ReadFileFormatHex(const std::string &path, const u32 line_offset, const u32 line_count);
            u64 GetDirectorySize(const std::string &path);

            inline void SetShouldWarnOnWriteAccess(const bool should_warn) {
                this->warn_write = should_warn;
            }

            inline bool ShouldWarnOnWriteAccess() {
                return this->warn_write;
            }

            virtual std::vector<std::string> GetDirectories(const std::string &path) = 0;
            virtual std::vector<std::string> GetFiles(const std::string &path) = 0;
            virtual bool Exists(const std::string &path) = 0;
            virtual bool IsFile(const std::string &path) = 0;
            virtual bool IsDirectory(const std::string &path) = 0;
            virtual void CreateFile(const std::string &path) = 0;
            virtual void CreateDirectory(const std::string &path) = 0;
            virtual void RenameFile(const std::string &path, const std::string &new_name) = 0;
            virtual void RenameDirectory(const std::string &path, const std::string &new_name) = 0;
            virtual void DeleteFile(const std::string &path) = 0;
            virtual void DeleteDirectory(const std::string &path) = 0;

            inline void EmptyDirectory(const std::string &path) {
                this->DeleteDirectory(path);
                this->CreateDirectory(path);
            }

            virtual void StartFileImpl(const std::string &path, const FileMode mode) = 0;
            virtual void EndFileImpl(const FileMode mode) = 0;

            inline bool HasStartedFile() {
                return this->started_file_mode != FileMode::None;
            }

            void StartFile(const std::string &path, const FileMode mode) {
                if(!this->HasStartedFile()) {
                    this->StartFileImpl(path, mode);
                    this->started_file_mode = mode;
                }
            }

            void EndFile() {
                if(this->HasStartedFile()) {
                    this->EndFileImpl(this->started_file_mode);
                    this->started_file_mode = FileMode::None;
                }
            }

            virtual u64 ReadFile(const std::string &path, u64 offset, u64 size, void *read_buf) = 0;
            virtual u64 WriteFile(const std::string &path, const void *write_buf, u64 size) = 0;
            virtual u64 GetFileSize(const std::string &path) = 0;

            virtual u64 GetTotalSpace() = 0;
            virtual u64 GetFreeSpace() = 0;
            virtual void SetArchiveBit(const std::string &path) = 0;
    };

}