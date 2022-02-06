
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
#include <usb/cmd/cmd_Base.hpp>
#include <fs/fs_Explorer.hpp>

namespace usb::cf {

    enum class PathType : u32 {
        Invalid = 0,
        File = 1,
        Directory = 2
    };

    inline Result GetDriveCount(u32 &out_count) {
        return cmd::ProcessCommand<1>(cmd::OutValue(out_count));
    }

    inline Result GetDriveInfo(const u32 drive_idx, std::string &out_label, std::string &out_path, size_t &out_total_size, size_t &out_free_size) {
        return cmd::ProcessCommand<2>(cmd::InValue(drive_idx), cmd::OutString(out_label), cmd::OutString(out_path), cmd::OutValue(out_total_size), cmd::OutValue(out_free_size));
    }

    inline Result StatPath(const std::string &path, PathType &out_type, size_t &out_file_size) {
        return cmd::ProcessCommand<3>(cmd::InString(path), cmd::OutValue(out_type), cmd::OutValue(out_file_size));
    }

    inline Result GetFileCount(const std::string &dir, u32 &out_count) {
        return cmd::ProcessCommand<4>(cmd::InString(dir), cmd::OutValue(out_count));
    }

    inline Result GetFile(const std::string &dir, const u32 file_idx, std::string &out_file) {
        return cmd::ProcessCommand<5>(cmd::InString(dir), cmd::InValue(file_idx), cmd::OutString(out_file));
    }

    inline Result GetDirectoryCount(const std::string &dir, u32 &out_count) {
        return cmd::ProcessCommand<6>(cmd::InString(dir), cmd::OutValue(out_count));
    }

    inline Result GetDirectory(const std::string &dir, const u32 subdir_idx, std::string &out_subdir) {
        return cmd::ProcessCommand<7>(cmd::InString(dir), cmd::InValue(subdir_idx), cmd::OutString(out_subdir));
    }

    inline Result StartFile(const std::string &file, const fs::FileMode mode) {
        return cmd::ProcessCommand<8>(cmd::InString(file), cmd::InValue(mode));
    }

    inline Result ReadFile(const std::string &file, const u64 offset, const u64 size, u64 &out_read_size, void *read_buf) {
        return cmd::ProcessCommand<9>(cmd::InString(file), cmd::InValue(offset), cmd::InValue(size), cmd::OutValue(out_read_size), cmd::OutBuffer(read_buf, size));
    }

    inline Result WriteFile(const std::string &file, const u64 size, u64 &out_write_size, const void *write_buf) {
        return cmd::ProcessCommand<10>(cmd::InString(file), cmd::InValue(size), cmd::OutValue(out_write_size), cmd::InBuffer(write_buf, size));
    }

    inline Result EndFile(const fs::FileMode mode) {
        return cmd::ProcessCommand<11>(cmd::InValue(mode));
    }

    inline Result Create(const std::string &path, const PathType type) {
        return cmd::ProcessCommand<12>(cmd::InString(path), cmd::InValue(type));
    }

    inline Result Delete(const std::string &path) {
        return cmd::ProcessCommand<13>(cmd::InString(path));
    }

    inline Result Rename(const std::string &path, const std::string &new_name) {
        return cmd::ProcessCommand<14>(cmd::InString(path), cmd::InString(new_name));
    }

    inline Result GetSpecialPathCount(u32 &out_count) {
        return cmd::ProcessCommand<15>(cmd::OutValue(out_count));
    }

    inline Result GetSpecialPath(const u32 special_path_idx, std::string &out_path_name, std::string &out_path) {
        return cmd::ProcessCommand<16>(cmd::InValue(special_path_idx), cmd::OutString(out_path_name), cmd::OutString(out_path));
    }

    inline Result SelectFile(std::string &out_file) {
        return cmd::ProcessCommand<17>(cmd::OutString(out_file));
    }

    // TODO: add a SelectDirectory command?

}