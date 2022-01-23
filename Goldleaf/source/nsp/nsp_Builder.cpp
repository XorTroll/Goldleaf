
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

#include <nsp/nsp_Builder.hpp>
#include <fs/fs_FileSystem.hpp>

namespace nsp {

    bool GenerateFrom(const std::string &input_path, const std::string &output_nsp, GenerateCallback cb_fn) {
        auto exp = fs::GetExplorerForPath(input_path);
        auto files = exp->GetFiles(input_path);
        PFS0Header header = {
            .magic = Magic,
            .file_count = static_cast<u32>(files.size()),
        };
        auto string_table = reinterpret_cast<char*>(fs::GetWorkBuffer());
        size_t string_table_size = 0;
        std::vector<PFS0File> file_entries;
        size_t base_offset = 0;
        for(auto &file: files) {
            PFS0File entry = {};
            entry.entry.offset = base_offset;
            entry.entry.string_table_offset = string_table_size;
            const auto file_size = exp->GetFileSize(input_path + "/" + file);
            entry.entry.size = file_size;
            entry.name = file;
            base_offset += file_size;
            strcpy(&string_table[string_table_size], file.c_str());
            string_table_size += file.length() + 1; // NUL terminated!
            file_entries.push_back(entry);
        }
        string_table_size = (string_table_size + 0x1F) &~ 0x1F;
        header.string_table_size = string_table_size;
        auto out_exp = fs::GetExplorerForPath(output_nsp);
        out_exp->StartFile(output_nsp, fs::FileMode::Write);
        out_exp->WriteFile(output_nsp, &header, sizeof(header));
        for(const auto &entry: file_entries) {
            out_exp->WriteFile(output_nsp, &entry.entry, sizeof(entry.entry));
        }
        out_exp->WriteFile(output_nsp, string_table, string_table_size);
        for(const auto &entry: file_entries) {
            auto to_read = entry.entry.size;
            auto buf = fs::GetWorkBuffer();
            size_t off = 0;
            const auto entry_path = input_path + "/" + entry.name;
            exp->StartFile(entry_path, fs::FileMode::Read);
            while(to_read) {
                auto read = exp->ReadFile(entry_path, off, std::min(to_read, fs::WorkBufferSize), buf);
                out_exp->WriteFile(output_nsp, buf, read);
                off += read;
                to_read -= read;
                cb_fn(off, base_offset);
            }
            exp->EndFile();
        }
        out_exp->EndFile();
        return true;
    }

}