
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
#include <fs/fs_FileSystem.hpp>
#include <nsp/nsp_Types.hpp>

namespace nsp {

    class PFS0 {
        private:
            std::string path;
            fs::Explorer *exp;
            u8 *string_table;
            u32 header_size;
            PFS0Header header;
            std::vector<PFS0File> files;
            bool ok;

        public:
            static constexpr u32 InvalidFileIndex = UINT32_MAX;

            NX_CONSTEXPR bool IsValidFileIndex(u32 idx) {
                return idx != InvalidFileIndex;
            }
            
            NX_CONSTEXPR bool IsInvalidFileIndex(u32 idx) {
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
