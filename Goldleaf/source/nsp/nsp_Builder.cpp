
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

#include <nsp/nsp_Builder.hpp>
#include <fs/fs_FileSystem.hpp>
#include <fs/fs_PFS0.hpp>

namespace nsp {

    bool GenerateFrom(const std::string &input_path, const std::string &output_nsp, GenerateStartCallback start_cb, GenerateProgressCallback prog_cb) {
        auto exp = fs::GetExplorerForPath(input_path);
        const auto files = exp->GetFiles(input_path);
        fs::PFS0::Header header = {
            .magic = fs::PFS0::Magic,
            .file_count = static_cast<u32>(files.size()),
        };

        const size_t string_table_max_size = files.size() * FS_MAX_PATH;
        auto string_table_buf = fs::AllocateWorkBuffer(string_table_max_size);
        size_t string_table_size = 0;
        std::vector<fs::PFS0::File> file_entries;
        size_t base_offset = 0;
        for(auto &file: files) {
            fs::PFS0::File entry = {};
            entry.entry.offset = base_offset;
            entry.entry.string_table_offset = string_table_size;
            const auto file_size = exp->GetFileSize(input_path + "/" + file);
            entry.entry.size = file_size;
            entry.name = file;
            base_offset += file_size;
            strcpy(reinterpret_cast<char*>(string_table_buf) + string_table_size, file.c_str());
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
        out_exp->WriteFile(output_nsp, string_table_buf, string_table_size);
        fs::DeleteWorkBuffer(string_table_buf);

        start_cb((double)base_offset);
        for(const auto &entry: file_entries) {
            auto rem_size = entry.entry.size;
            auto work_buf = fs::AllocateWorkBuffer();
            size_t off = 0;
            const auto entry_path = input_path + "/" + entry.name;
            exp->StartFile(entry_path, fs::FileMode::Read);
            while(rem_size > 0) {
                const auto read_size = exp->ReadFile(entry_path, off, std::min(rem_size, fs::DefaultWorkBufferSize), work_buf);
                out_exp->WriteFile(output_nsp, work_buf, read_size);
                off += read_size;
                rem_size -= read_size;
                prog_cb((double)read_size);
            }
            exp->EndFile();
            fs::DeleteWorkBuffer(work_buf);
        }
        out_exp->EndFile();
        return true;
    }

}
