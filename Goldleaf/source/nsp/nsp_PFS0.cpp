
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

#include <nsp/nsp_PFS0.hpp>

namespace nsp {

    PFS0::PFS0(fs::Explorer *exp, const std::string &path) {
        this->path = path;
        this->exp = exp;
        this->ok = false;
        this->header_size = 0;
        this->string_table = nullptr;
        this->exp->StartFile(this->path, fs::FileMode::Read);
        this->exp->ReadFile(this->path, 0, sizeof(this->header), &this->header);
        if(this->header.magic == Magic) {
            this->ok = true;
            const auto string_table_offset = sizeof(PFS0Header) + (sizeof(PFS0FileEntry) * this->header.file_count);
            this->string_table = new u8[this->header.string_table_size]();
            this->header_size = string_table_offset + this->header.string_table_size;
            this->exp->ReadFile(this->path, string_table_offset, this->header.string_table_size, this->string_table);
            for(u32 i = 0; i < this->header.file_count; i++) {
                const auto offset = sizeof(PFS0Header) + (i * sizeof(PFS0FileEntry));
                PFS0FileEntry ent = {};
                this->exp->ReadFile(this->path, offset, sizeof(ent), &ent);
                std::string name;
                for(u32 j = ent.string_table_offset; j < this->header.string_table_size; j++) {
                    const auto ch = static_cast<char>(this->string_table[j]);
                    if(ch == '\0') {
                        break;
                    }
                    name += ch;
                }
                const PFS0File file = {
                    .entry = ent,
                    .name = name,
                };
                this->files.push_back(file);
            }
        }
        this->exp->EndFile();
    }

    PFS0::~PFS0() {
        if(this->string_table != nullptr) {
            delete[] this->string_table;
            this->string_table = nullptr;
        }
    }

    std::string PFS0::GetFile(const u32 idx) {
        if(IsInvalidFileIndex(idx)) {
            return "";
        }
        if(idx >= this->files.size()) {
            return "";
        }
        return this->files[idx].name;
    }

    u64 PFS0::ReadFromFile(const u32 idx, const u64 offset, const u64 size, void *read_buf) {
        return this->exp->ReadFile(this->path, (this->header_size + this->files[idx].entry.offset + offset), size, read_buf);
    }

    std::vector<std::string> PFS0::GetFiles() {
        std::vector<std::string> pfiles;
        for(const auto &file: this->files) {
            pfiles.push_back(file.name);
        }
        return pfiles;
    }

    u64 PFS0::GetFileSize(const u32 idx) {
        if(IsInvalidFileIndex(idx)) {
            return 0;
        }
        if(idx >= this->files.size()) {
            return 0;
        }
        return this->files[idx].entry.size;
    }

    void PFS0::SaveFile(const u32 idx, fs::Explorer *path_exp, const std::string &path) {
        if(IsInvalidFileIndex(idx)) {
            return;
        }
        if(idx >= this->files.size()) {
            return;
        }
        const auto file_size = this->GetFileSize(idx);
        auto buf = fs::GetWorkBuffer();
        auto rem_size = file_size;
        u64 off = 0;
        path_exp->DeleteFile(path);
        path_exp->CreateFile(path);
        this->exp->StartFile(this->path, fs::FileMode::Read);
        path_exp->StartFile(path, fs::FileMode::Write);
        while(rem_size) {
            const auto read_size = this->ReadFromFile(idx, off, std::min(fs::WorkBufferSize, rem_size), buf);
            path_exp->WriteFile(path, buf, read_size);
            off += read_size;
            rem_size -= read_size;
        }
        this->exp->EndFile();
        path_exp->EndFile();
    }

    u32 PFS0::GetFileIndexByName(const std::string &file_name) {
        auto found = false;
        u32 idx = 0;
        for(auto &pfs0_file: this->files) {
            if(strcasecmp(pfs0_file.name.c_str(), file_name.c_str()) == 0) {
                return idx;
            }
            idx++;
        }
        return InvalidFileIndex;
    }

}