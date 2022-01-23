
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

#include <fs/fs_RemotePCExplorer.hpp>
#include <usb/usb_Commands.hpp>

namespace fs {

    RemotePCExplorer::RemotePCExplorer(const std::string &mount_name) {
        this->SetNames(mount_name, mount_name);
    }

    std::vector<std::string> RemotePCExplorer::GetDirectories(const std::string &path) {
        std::vector<std::string> dirs;
        const auto full_path = this->MakeFull(path);

        u32 dir_count = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetDirectoryCount>(usb::InString(full_path), usb::OutValue(dir_count)))) {
            dirs.reserve(dir_count);
            for(u32 i = 0; i < dir_count; i++) {
                std::string dir;
                if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetDirectory>(usb::InString(full_path), usb::InValue(i), usb::OutString(dir)))) {
                    dirs.push_back(dir);
                }
            }
        }
        return dirs;
    }

    std::vector<std::string> RemotePCExplorer::GetFiles(const std::string &path) {
        std::vector<std::string> files;
        const auto full_path = this->MakeFull(path);

        u32 file_count = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetFileCount>(usb::InString(full_path), usb::OutValue(file_count)))) {
            files.reserve(file_count);
            for(u32 i = 0; i < file_count; i++) {
                std::string file;
                if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetFile>(usb::InString(full_path), usb::InValue(i), usb::OutString(file)))) {
                    files.push_back(file);
                }
            }
        }
        return files;
    }

    bool RemotePCExplorer::Exists(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        u32 type = 0;
        size_t tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::OutValue(type), usb::OutValue(tmp_file_size)))) {
            return (type == 1) || (type == 2);
        }
        else {
            return false;
        }
    }

    bool RemotePCExplorer::IsFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        u32 type = 0;
        size_t tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::OutValue(type), usb::OutValue(tmp_file_size)))) {
            return type == 1;
        }
        return false;
    }

    bool RemotePCExplorer::IsDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        u32 type = 0;
        size_t tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::OutValue(type), usb::OutValue(tmp_file_size)))) {
            return type == 2;
        }
        return false;
    }

    void RemotePCExplorer::CreateFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Create>(usb::InValue<u32>(1), usb::InString(full_path));
    }

    void RemotePCExplorer::CreateDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Create>(usb::InValue<u32>(2), usb::InString(full_path));
    }

    void RemotePCExplorer::RenameFile(const std::string &path, const std::string &new_name) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Rename>(usb::InValue<u32>(1), usb::InString(full_path), usb::InString(new_name));
    }

    void RemotePCExplorer::RenameDirectory(const std::string &path, const std::string &new_name) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Rename>(usb::InValue<u32>(2), usb::InString(full_path), usb::InString(new_name));
    }

    void RemotePCExplorer::DeleteFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Delete>(usb::InValue<u32>(1), usb::InString(full_path));
    }

    void RemotePCExplorer::DeleteDirectory(const std::string &path) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::Delete>(usb::InValue<u32>(2), usb::InString(full_path));
    }

    void RemotePCExplorer::StartFileImpl(const std::string &path, const FileMode mode) {
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::StartFile>(usb::InString(full_path), usb::InValue(mode));
    }

    void RemotePCExplorer::EndFileImpl(const FileMode mode) {
        usb::ProcessCommand<usb::CommandId::EndFile>(usb::InValue(mode));
    }

    u64 RemotePCExplorer::ReadFile(const std::string &path, const u64 offset, const u64 size, void *read_buf) {
        u64 read_size = 0;
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::ReadFile>(usb::InString(full_path), usb::InValue(offset), usb::InValue(size), usb::OutValue(read_size), usb::OutBuffer(read_buf, size));
        return read_size;
    }

    u64 RemotePCExplorer::WriteFile(const std::string &path, const void *write_buf, const u64 size) {
        const auto full_path = this->MakeFull(path);

        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::WriteFile>(usb::InString(full_path), usb::InValue(size), usb::InBuffer(write_buf, size)))) {
            return size;
        }
        else {
            return 0;
        }
    }

    u64 RemotePCExplorer::GetFileSize(const std::string &path) {
        u32 tmp_type = 0;
        size_t file_size = 0;
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::OutValue(tmp_type), usb::OutValue(file_size));
        return file_size;
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