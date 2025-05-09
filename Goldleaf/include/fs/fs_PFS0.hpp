
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

#pragma once
#include <fs/fs_FileSystem.hpp>

namespace fs {

    class PFS0 {
        public:
            struct Header {
                u32 magic;
                u32 file_count;
                u32 string_table_size;
                u32 reserved;
            };
            static_assert(sizeof(Header) == 0x10);

            struct FileEntry {
                u64 offset;
                u64 size;
                u32 string_table_offset;
                u32 pad;
            };
            static_assert(sizeof(FileEntry) == 0x18);

            static constexpr u32 Magic = 0x30534650; // 'PFS0'

            static constexpr u32 InvalidFileIndex = UINT32_MAX;

            struct File {
                FileEntry entry;
                std::string name;
            };

        private:
            std::string path;
            fs::Explorer *exp;
            u8 *string_table;
            u32 header_size;
            Header header;
            std::vector<File> files;
            bool ok;

        public:
            static constexpr inline bool IsValidFileIndex(u32 idx) {
                return idx != InvalidFileIndex;
            }
            
            static constexpr inline bool IsInvalidFileIndex(u32 idx) {
                return idx == InvalidFileIndex;
            }

            PFS0(fs::Explorer *explorer, const std::string &path);
            ~PFS0();
            
            inline u32 GetCount() {
                return this->header.file_count;
            }
            
            std::string GetFile(const u32 idx);
            
            inline std::string GetPath() {
                return this->path;
            }
            
            u64 ReadFromFile(const u32 idx, const u64 offset, const u64 size, void *read_buf);
            std::vector<std::string> GetFiles();
            
            inline bool IsOk() {
                return this->ok;
            }
            
            inline fs::Explorer *GetExplorer() {
                return this->exp;
            }

            u64 GetFileSize(const u32 idx);
            void SaveFile(const u32 idx, fs::Explorer *path_exp, const std::string &path);
            u32 GetFileIndexByName(const std::string &file_name);
    };

}
