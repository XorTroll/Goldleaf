
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
#include <fs/fs_Common.hpp>

namespace fs {

    enum class FileMode : u32 {
        None,
        Read,
        Write,
        Append,
    };

    class Explorer {
        protected:
            String disp_name;
            String mnt_name;
            String cwd;
            bool warn_write;
            FileMode started_file_mode;

        public:
            Explorer() : warn_write(false), started_file_mode(FileMode::None) {}
            virtual ~Explorer() {}

            void SetNames(String mount_name, String display_name);
            bool NavigateBack();
            bool NavigateForward(String path);
            std::vector<String> GetContents();
            String GetMountName();
            String GetCwd();
            String GetPresentableCwd();
        
            inline String FullPathFor(String path) {
                auto new_path = this->cwd;
                if(this->cwd.substr(this->cwd.length() - 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline String AbsolutePathFor(String path) {
                auto new_path = this->mnt_name + ":";
                if(path.substr(0, 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline String FullPresentablePathFor(String path) {
                auto pres_cwd = this->GetPresentableCwd();
                auto new_path = pres_cwd;
                if(pres_cwd.substr(pres_cwd.length() - 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline String AbsolutePresentablePathFor(String path) {
                auto new_path = this->disp_name + ":";
                if(path.substr(0, 1) != "/") {
                    new_path += "/";
                }
                new_path += path;
                return new_path;
            }

            inline String MakeFull(String path) {
                return this->IsFullPath(path) ? path : this->FullPathFor(path);
            }

            inline String MakeFullPresentable(String path) {
                return this->IsFullPath(path) ? path : this->FullPresentablePathFor(path);
            }

            inline String MakeAbsolute(String path) {
                return this->IsFullPath(path) ? path : this->AbsolutePathFor(path);
            }

            inline String MakeAbsolutePresentable(String path) {
                return this->IsFullPath(path) ? path : this->AbsolutePresentablePathFor(path);
            }

            inline String RemoveMountName(String path) {
                return path.substr(this->mnt_name.length() + 1);
            }

            inline bool IsFullPath(String path) {
                return path.find(":/") != String::npos;
            }

            void CopyFile(String path, String new_path);
            void CopyFileProgress(String path, String new_path, std::function<void(double Done, double Total)> cb_fn);
            void CopyDirectory(String dir, String new_dir);
            void CopyDirectoryProgress(String dir, String new_dir, std::function<void(double Done, double Total)> cb_fn);
            bool IsFileBinary(String path);
            std::vector<u8> ReadFile(String path);
            JSON ReadJSON(String path);

            inline void WriteJSON(const JSON &json, String path) {
                const auto json_str = json.dump(4);
                this->WriteFile(path, json_str.c_str(), json_str.length());
            }

            std::vector<String> ReadFileLines(String path, u32 line_offset, u32 line_count);
            std::vector<String> ReadFileFormatHex(String path, u32 line_offset, u32 line_count);
            u64 GetDirectorySize(String path);

            inline void SetShouldWarnOnWriteAccess(bool should_warn) {
                this->warn_write = should_warn;
            }

            inline bool ShouldWarnOnWriteAccess() {
                return this->warn_write;
            }

            virtual std::vector<String> GetDirectories(String path) = 0;
            virtual std::vector<String> GetFiles(String path) = 0;
            virtual bool Exists(String path) = 0;
            virtual bool IsFile(String path) = 0;
            virtual bool IsDirectory(String path) = 0;
            virtual void CreateFile(String path) = 0;
            virtual void CreateDirectory(String path) = 0;
            virtual void RenameFile(String path, String new_name) = 0;
            virtual void RenameDirectory(String path, String new_name) = 0;
            virtual void DeleteFile(String path) = 0;
            virtual void DeleteDirectory(String path) = 0;

            inline void EmptyDirectory(String path) {
                this->DeleteDirectory(path);
                this->CreateDirectory(path);
            }

            virtual void StartFileImpl(String path, FileMode mode) = 0;
            virtual void EndFileImpl(FileMode mode) = 0;

            inline bool HasStartedFile() {
                return this->started_file_mode != FileMode::None;
            }

            void StartFile(String path, FileMode mode) {
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

            virtual u64 ReadFile(String path, u64 offset, u64 size, void *read_buf) = 0;
            virtual u64 WriteFile(String path, const void *write_buf, u64 size) = 0;
            virtual u64 GetFileSize(String path) = 0;

            virtual u64 GetTotalSpace() = 0;
            virtual u64 GetFreeSpace() = 0;
            virtual void SetArchiveBit(String path) = 0;
    };

}