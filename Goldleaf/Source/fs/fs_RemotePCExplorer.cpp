
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

    RemotePCExplorer::RemotePCExplorer(String mount_name) {
        this->SetNames(mount_name, mount_name);
    }

    std::vector<String> RemotePCExplorer::GetDirectories(String path) {
        std::vector<String> dirs;
        const auto full_path = this->MakeFull(path);

        u32 dir_count = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetDirectoryCount>(usb::InString(full_path), usb::Out32(dir_count)))) {
            dirs.reserve(dir_count);
            for(u32 i = 0; i < dir_count; i++) {
                String dir;
                if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetDirectory>(usb::InString(full_path), usb::In32(i), usb::OutString(dir)))) {
                    dirs.push_back(dir);
                }
            }
        }
        return dirs;
    }

    std::vector<String> RemotePCExplorer::GetFiles(String path) {
        std::vector<String> files;
        const auto full_path = this->MakeFull(path);

        u32 file_count = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetFileCount>(usb::InString(full_path), usb::Out32(file_count)))) {
            files.reserve(file_count);
            for(u32 i = 0; i < file_count; i++) {
                String file;
                if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::GetFile>(usb::InString(full_path), usb::In32(i), usb::OutString(file)))) {
                    files.push_back(file);
                }
            }
        }
        return files;
    }

    bool RemotePCExplorer::Exists(String path) {
        const auto full_path = this->MakeFull(path);
        u32 type = 0;
        u64 tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::Out32(type), usb::Out64(tmp_file_size)))) {
            return (type == 1) || (type == 2);
        }
        return false;
    }

    bool RemotePCExplorer::IsFile(String path) {
        const auto full_path = this->MakeFull(path);
        u32 type = 0;
        u64 tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::Out32(type), usb::Out64(tmp_file_size)))) {
            return type == 1;
        }
        return false;
    }

    bool RemotePCExplorer::IsDirectory(String path) {
        const auto full_path = this->MakeFull(path);
        u32 type = 0;
        u64 tmp_file_size = 0;
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::Out32(type), usb::Out64(tmp_file_size)))) {
            return type == 2;
        }
        return false;
    }

    void RemotePCExplorer::CreateFile(String path) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(1), usb::InString(full_path));
    }

    void RemotePCExplorer::CreateDirectory(String path) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(2), usb::InString(full_path));
    }

    void RemotePCExplorer::RenameFile(String path, String new_name) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(1), usb::InString(full_path), usb::InString(new_name));
    }

    void RemotePCExplorer::RenameDirectory(String path, String new_name) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(2), usb::InString(full_path), usb::InString(new_name));
    }

    void RemotePCExplorer::DeleteFile(String path) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(1), usb::InString(full_path));
    }

    void RemotePCExplorer::DeleteDirectory(String path) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(2), usb::InString(full_path));
    }

    void RemotePCExplorer::StartFileImpl(String path, FileMode mode) {
        const auto full_path = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::StartFile>(usb::InString(full_path), usb::In32((u32)mode));
    }

    void RemotePCExplorer::EndFileImpl(FileMode mode) {
        usb::ProcessCommand<usb::CommandId::EndFile>(usb::In32(static_cast<u32>(mode)));
    }

    u64 RemotePCExplorer::ReadFile(String path, u64 offset, u64 size, void *read_buf) {
        u64 read_size = 0;
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::ReadFile>(usb::InString(full_path), usb::In64(offset), usb::In64(size), usb::Out64(read_size), usb::OutBuffer(read_buf, size));
        return read_size;
    }

    u64 RemotePCExplorer::WriteFile(String path, const void *write_buf, u64 size) {
        const auto full_path = this->MakeFull(path);
        if(R_SUCCEEDED(usb::ProcessCommand<usb::CommandId::WriteFile>(usb::InString(full_path), usb::In64(size), usb::InBuffer(write_buf, size)))) {
            return size;
        }
        return 0;
    }

    u64 RemotePCExplorer::GetFileSize(String path) {
        u32 tmp_type = 0;
        u64 file_size = 0;
        const auto full_path = this->MakeFull(path);

        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(full_path), usb::Out32(tmp_type), usb::Out64(file_size));
        return file_size;
    }

    u64 RemotePCExplorer::GetTotalSpace() {
        // TODO
        return 0;
    }

    u64 RemotePCExplorer::GetFreeSpace() {
        // TODO
        return 0;
    }

    void RemotePCExplorer::SetArchiveBit(String path) {
        // Non-HOS operating systems don't handle archive bit for what we want, so this is stubbed.
    }
}