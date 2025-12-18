
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#include <fs/fs_FileSystem.hpp>

namespace fs {

    namespace {

        bool InternalCaseCompare(const std::string &a, const std::string &b) {
            auto a_copy = a;
            auto b_copy = b;
            std::transform(a_copy.begin(), a_copy.end(), a_copy.begin(), ::tolower);
            std::transform(b_copy.begin(), b_copy.end(), b_copy.begin(), ::tolower);
            return a_copy < b_copy;
        }
        
        NX_CONSTEXPR bool IsCharacterText(const char ch) {
            return isascii(ch) && (isprint(ch) || (ch == '\n') || (ch == '\r') || (ch == '\t'));
        }

        constexpr size_t BinaryCheckWorkBufferSize = 0x200; // Same size as GodMode9
        alignas(WorkBufferAlignment) u8 g_BinaryCheckWorkBuffer[BinaryCheckWorkBufferSize] = {};

    }

    void Explorer::SetNames(const std::string &mount_name, const std::string &display_name) {
        this->disp_name = display_name;
        this->mnt_name = mount_name;
        this->cwd = mount_name + ":/";
    }

    bool Explorer::NavigateBack() {
        if(this->cwd == (this->mnt_name + ":/")) {
            return false;
        }
        auto parent = this->cwd.substr(0, this->cwd.find_last_of("/\\"));
        if(parent.substr(parent.length() - 1) == ":") {
            parent += "/";
        }
        this->cwd = parent;
        return true;
    }

    bool Explorer::NavigateForward(const std::string &path) {
        const auto is_dir = this->IsDirectory(path);
        if(is_dir) {
            this->cwd = this->MakeFull(path);
        }
        return is_dir;
    }

    std::vector<std::string> Explorer::GetContents() {
        auto dirs = this->GetDirectories(this->cwd);
        auto files = this->GetFiles(this->cwd);

        if(!dirs.empty()) {
            std::sort(dirs.begin(), dirs.end(), InternalCaseCompare);
        }
        if(!files.empty()) {
            std::sort(files.begin(), files.end(), InternalCaseCompare);
            dirs.insert(dirs.end(), files.begin(), files.end());
        }
        return dirs;
    }

    std::string Explorer::GetPresentableCwd() const {
        if(this->cwd == (this->mnt_name + ":/")) {
            return this->disp_name + ":/";
        }
        const auto mnt_root_size = this->mnt_name.length() + 2;
        return this->disp_name + ":/" + this->cwd.substr(mnt_root_size);
    }

    void Explorer::CopyFile(const std::string &path, const std::string &new_path) {
        const auto full_path = this->MakeFull(path);
        auto exp = GetExplorerForPath(new_path);
        const auto full_new_path = exp->MakeFull(new_path);
        auto work_buf = AllocateWorkBuffer();
        auto rem_size = this->GetFileSize(full_path);
        u64 offset = 0;
        this->StartFile(full_path, fs::FileMode::Read);
        exp->StartFile(full_new_path, fs::FileMode::Write);
        while(rem_size) {
            const auto read_size = this->ReadFile(full_path, offset, std::min(rem_size, DefaultWorkBufferSize), work_buf);
            rem_size -= read_size;
            offset += read_size;
            exp->WriteFile(new_path, work_buf, read_size);
        }
        DeleteWorkBuffer(work_buf);
        this->EndFile();
        exp->EndFile();
    }

    void Explorer::CopyDirectory(const std::string &dir, const std::string &new_dir) {
        const auto full_dir = this->MakeFull(dir);
        auto exp = GetExplorerForPath(new_dir);
        const auto full_new_dir = exp->MakeFull(new_dir);
        exp->CreateDirectory(full_new_dir);
        const auto dirs = this->GetDirectories(full_dir);
        for(const auto &sub_dir: dirs) {
            const auto from = full_dir + "/" + sub_dir;
            const auto to = full_new_dir + "/" + sub_dir;
            this->CopyDirectory(from, to);
        }
        const auto files = this->GetFiles(full_dir);
        for(const auto &sub_file: files) {
            const auto from = full_dir + "/" + sub_file;
            const auto to = full_new_dir + "/" + sub_file;
            this->CopyFile(from, to);
        }
    }

    void Explorer::CopyFileProgress(const std::string &path, const std::string &new_path, CopyFileStartCallback start_cb, CopyFileProgressCallback prog_cb) {
        const auto full_path = this->MakeFull(path);
        auto exp = GetExplorerForPath(new_path);
        const auto full_new_path = exp->MakeFull(new_path);
        auto work_buf = AllocateWorkBuffer();
        const auto file_size = this->GetFileSize(full_path);
        start_cb(file_size);
        auto rem_size = file_size;
        u64 offset = 0;
        this->StartFile(full_path, fs::FileMode::Read);
        exp->StartFile(full_new_path, fs::FileMode::Write);
        while(rem_size) {
            const auto read_size = this->ReadFile(full_path, offset, std::min(rem_size, DefaultWorkBufferSize), work_buf);
            rem_size -= read_size;
            offset += read_size;
            exp->WriteFile(full_new_path, work_buf, read_size);
            prog_cb(read_size);
        }
        DeleteWorkBuffer(work_buf);
        this->EndFile();
        exp->EndFile();
    }

    namespace {

        void DoCopyDirectoryProgress(Explorer *exp, u64 &cur_done_size, const u64 total_size, const std::string &dir, const std::string &new_dir, CopyDirectoryFileStartCallback file_start_cb, CopyDirectoryFileProgressCallback file_prog_cb) {
            const auto full_dir = exp->MakeFull(dir);
            auto new_exp = GetExplorerForPath(new_dir);
            const auto full_new_dir = new_exp->MakeFull(new_dir);
            new_exp->CreateDirectory(full_new_dir);

            for(const auto &copy_file_name: exp->GetFiles(full_dir)) {
                const auto copy_file = full_dir + "/" + copy_file_name;
                const auto new_file = full_new_dir + "/" + copy_file_name;
                exp->CopyFileProgress(copy_file, new_file, [&](const size_t file_size) {
                    file_start_cb(file_size, copy_file, new_file);
                }, [&](const size_t cur_rw_size) {
                    file_prog_cb(cur_rw_size);
                });
                cur_done_size += exp->GetFileSize(copy_file);
            }

            for(const auto &copy_dir_name: exp->GetDirectories(full_dir)) {
                const auto copy_dir = full_dir + "/" + copy_dir_name;
                const auto new_dir = full_new_dir + "/" + copy_dir_name;
                DoCopyDirectoryProgress(exp, cur_done_size, total_size, copy_dir, new_dir, file_start_cb, file_prog_cb);
            }
        }

    }

    void Explorer::CopyDirectoryProgress(const std::string &dir, const std::string &new_dir, CopyDirectoryStartCallback start_cb, CopyDirectoryFileStartCallback file_start_cb, CopyDirectoryFileProgressCallback file_prog_cb) {
        u64 cur_done_size = 0;
        const auto total_size = this->GetDirectorySize(dir);
        start_cb(total_size);

        DoCopyDirectoryProgress(this, cur_done_size, total_size, dir, new_dir, file_start_cb, file_prog_cb);
    }

    bool Explorer::IsFileBinary(const std::string &path) {
        const auto full_path = this->MakeFull(path);
        if(!this->IsFile(full_path)) {
            return false;
        }
        const auto file_size = this->GetFileSize(full_path);
        if(file_size == 0) {
            return true;
        }

        const auto to_read_size = std::min(file_size, BinaryCheckWorkBufferSize);
        const auto read_size = this->ReadFile(full_path, 0, to_read_size, g_BinaryCheckWorkBuffer);
        if(read_size == 0) {
            return true;
        }
        else {
            for(u32 i = 0; i < read_size; i++) {
                const auto cur_ch = static_cast<char>(g_BinaryCheckWorkBuffer[i]);
                if(!IsCharacterText(cur_ch)) {
                    return true;
                }
            }
            return false;
        }
    }

    std::vector<u8> Explorer::ReadFile(const std::string &path) {
        const auto full_path = this->MakeFull(path);
        const auto file_size = this->GetFileSize(full_path);

        std::vector<u8> data;
        if(file_size > 0) {
            data.reserve(file_size);
            this->ReadFile(full_path, 0, file_size, data.data());
        }
        return data;
    }

    std::vector<std::string> Explorer::ReadFileLines(const std::string &path, const u32 line_offset, const u32 line_count) {
        std::vector<std::string> data;
        const auto full_path = this->MakeFull(path);
        const auto file_size = this->GetFileSize(full_path);
        if(file_size == 0) {
            return data;
        }
        std::string tmp_line;
        u32 tmp_line_offset = 0;
        auto rem_size = file_size;
        u64 offset = 0;
        auto work_buf = AllocateWorkBuffer();
        auto end = false;
        while(rem_size && !end) {
            const auto read_size = this->ReadFile(full_path, offset, std::min(DefaultWorkBufferSize, rem_size), work_buf);
            if(read_size == 0) {
                return data;
            }
            rem_size -= read_size;
            offset += read_size;
            for(u32 i = 0; i < read_size; i++) {
                const auto ch = static_cast<char>(work_buf[i]);
                if(ch == '\n') {
                    const auto prev_tmp_line_offset = tmp_line_offset;
                    tmp_line_offset++;
                    if(prev_tmp_line_offset < line_offset) {
                        tmp_line = "";
                        continue;
                    }
                    std::string tab = "\t";
                    while(true) {
                        const auto find_pos = tmp_line.find(tab);
                        if(find_pos == std::string::npos) {
                            break;
                        }
                        tmp_line.replace(find_pos, tab.length(), "    ");
                    }
                    data.push_back(tmp_line);
                    if(data.size() >= line_count) {
                        end = true;
                        break;
                    }
                    tmp_line = "";
                }
                else if(tmp_line_offset >= line_offset) {
                    tmp_line += ch;
                }
            }
        }
        DeleteWorkBuffer(work_buf);
        if(!tmp_line.empty()) {
            data.push_back(tmp_line);
            tmp_line = "";
        }
        return data;
    }

    std::vector<std::string> Explorer::ReadFileFormatHex(const std::string &path, const u32 line_offset, const u32 line_count) {
        std::vector<std::string> str_data;
        const auto full_path = this->MakeFull(path);
        const auto file_size = this->GetFileSize(full_path);
        const auto offset = 16 * line_offset;
        const u64 read_size = 16 * line_count;
        if(offset >= file_size) {
            return str_data;
        }
        auto data_read_size = std::min(file_size, read_size);
        if((offset + read_size) > file_size) {
            data_read_size = read_size - ((offset + read_size) - file_size);
        }
        auto work_buf = AllocateWorkBuffer();
        this->ReadFile(full_path, offset, data_read_size, work_buf);
        u32 count = 0;
        std::string tmp_line;
        std::string tmp_chr;
        u32 tmp_offset = 0;
        for(u32 i = 0; i < (data_read_size + 1); i++) {
            if(count == 16) {
                std::stringstream strm;
                strm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (offset + tmp_offset);
                const auto def = " " + strm.str() + "   " + tmp_line + "  " + tmp_chr;
                str_data.push_back(def);
                tmp_offset += 16;
                count = 0;
                tmp_line = "";
                tmp_chr = "";
            }
            else if(i == data_read_size) {
                if((data_read_size % 16) != 0) {
                    const auto miss = 16 - count;
                    for(u32 i = 0; i < miss; i++) {
                        tmp_line += "   ";
                        tmp_chr += " ";
                    }
                }
                std::stringstream strm;
                strm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (offset + tmp_offset);
                const auto def = " " + strm.str() + "   " + tmp_line + "  " + tmp_chr;
                str_data.push_back(def);
                break;
            }
            const auto cur_byte = work_buf[i];
            std::stringstream strm;
            strm << std::setw(2) << std::uppercase << std::setfill('0') << std::hex << static_cast<u32>(cur_byte);
            tmp_line += strm.str() + " ";
            if(isprint(cur_byte)) {
                tmp_chr += static_cast<char>(cur_byte);
            }
            else {
                tmp_chr += ".";
            }
            count++;
        }
        DeleteWorkBuffer(work_buf);
        return str_data;
    }

    u64 Explorer::GetDirectorySize(const std::string &path) {
        u64 size = 0;
        const auto full_path = this->MakeFull(path);

        const auto dirs = this->GetDirectories(full_path);
        for(const auto &dir: dirs) {
            size += this->GetDirectorySize(full_path + "/" + dir);
        }
        
        const auto files = this->GetFiles(full_path);
        for(const auto &file: files) {
            size += this->GetFileSize(full_path + "/" + file);
        }
        return size;
    }

}
