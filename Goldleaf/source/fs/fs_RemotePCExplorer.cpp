
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

#include <fs/fs_RemotePCExplorer.hpp>
#include <usb/cf/cf_CommandFramework.hpp>

namespace fs {

    RemotePCExplorer::RemotePCExplorer(const std::string &mount_name) {
        this->SetNames(mount_name, mount_name);
    }

    std::vector<std::string> RemotePCExplorer::GetDirectories(const std::string &path) {
        std::vector<std::string> dirs;
        const auto full_path = this->MakeFull(path);

        u32 dir_count = 0;
        if(R_SUCCEEDED(usb::cf::GetDirectoryCount(full_path, dir_count))) {
            dirs.reserve(dir_count);
            for(u32 i = 0; i < dir_count; i++) {
                std::string dir;
                if(R_SUCCEEDED(usb::cf::GetDirectory(full_path, i, dir))) {
                    dirs.push_back(dir);
                }
            }
        }
        return dirs;
    }

    std::vector<std::string> RemotePCExplorer::GetFiles(const std::string &path) {
        std::vector<std::string> files;
        const auto full_path = this->MakeFull(path);

        u32 file_count;
        if(R_SUCCEEDED(usb::cf::GetFileCount(full_path, file_count))) {
            files.reserve(file_count);
            for(u32 i = 0; i < file_count; i++) {
                std::string file;
                if(R_SUCCEEDED(usb::cf::GetFile(full_path, i, file))) {
                    files.push_back(file);
                }
            }
        }
        return files;
    }

    bool RemotePCExplorer::Exists(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::PathType type;
        size_t tmp_file_size;
        if(R_SUCCEEDED(usb::cf::StatPath(full_path, type, tmp_file_size))) {
            return (type == usb::cf::PathType::File) || (type == usb::cf::PathType::Directory);
        }
        else {
            return false;
        }
    }

    bool RemotePCExplorer::IsFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::PathType type;
        size_t tmp_file_size;
        if(R_SUCCEEDED(usb::cf::StatPath(full_path, type, tmp_file_size))) {
            return type == usb::cf::PathType::File;
        }
        else {
            return false;
        }
    }

    bool RemotePCExplorer::IsDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::PathType type;
        size_t tmp_file_size;
        if(R_SUCCEEDED(usb::cf::StatPath(full_path, type, tmp_file_size))) {
            return type == usb::cf::PathType::Directory;
        }
        else {
            return false;
        }
    }

    void RemotePCExplorer::CreateFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Create(full_path, usb::cf::PathType::File);
    }

    void RemotePCExplorer::CreateDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Create(full_path, usb::cf::PathType::Directory);
    }

    void RemotePCExplorer::RenameFile(const std::string &path, const std::string &new_name) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Rename(full_path, new_name);
    }

    void RemotePCExplorer::RenameDirectory(const std::string &path, const std::string &new_name) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Rename(full_path, new_name);
    }

    void RemotePCExplorer::DeleteFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Delete(full_path);
    }

    void RemotePCExplorer::DeleteDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::cf::Delete(full_path);
    }

    void RemotePCExplorer::StartFileImpl(const std::string &path, const FileMode mode) {
        const auto full_path = this->MakeFull(path);

        usb::cf::StartFile(full_path, mode);
    }

    void RemotePCExplorer::EndFileImpl(const FileMode mode) {
        usb::cf::EndFile(mode);
    }

    u64 RemotePCExplorer::ReadFile(const std::string &path, const u64 offset, const u64 size, void *read_buf) {
        const auto full_path = this->MakeFull(path);

        u64 read_size;
        if(R_SUCCEEDED(usb::cf::ReadFile(full_path, offset, size, read_size, read_buf))) {
            return read_size;
        }
        else {
            return 0;
        }
    }

    u64 RemotePCExplorer::WriteFile(const std::string &path, const void *write_buf, const u64 size) {
        const auto full_path = this->MakeFull(path);

        u64 write_size;
        if(R_SUCCEEDED(usb::cf::WriteFile(full_path, size, write_size, write_buf))) {
            return write_size;
        }
        else {
            return 0;
        }
    }

    u64 RemotePCExplorer::GetFileSize(const std::string &path) {
        const auto full_path = this->MakeFull(path);
        
        usb::cf::PathType tmp_type;
        size_t file_size;
        if(R_SUCCEEDED(usb::cf::StatPath(full_path, tmp_type, file_size))) {
            return file_size;
        }
        else {
            return 0;
        }
    }

    u64 RemotePCExplorer::GetTotalSpace() {
        // TODO?
        return 0;
    }

    u64 RemotePCExplorer::GetFreeSpace() {
        // TODO?
        return 0;
    }

    void RemotePCExplorer::SetArchiveBit(const std::string &path) {
        // Non-HOS operating systems don't handle archive bit for what we want, so this is stubbed
    }

}