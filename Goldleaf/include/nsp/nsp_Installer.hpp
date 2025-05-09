
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
#include <fs/fs_PFS0.hpp>
#include <cnt/cnt_PackagedContentMeta.hpp>
#include <cnt/cnt_Content.hpp>
#include <cnt/cnt_Ticket.hpp>

namespace nsp {

    struct ContentWriteProgressEntry {
        NcmContentType type;
        NcmPlaceHolderId placehld_id;
        size_t cur_offset;
        size_t size;
    };
    
    struct ContentWriteProgress {
        std::vector<ContentWriteProgressEntry> entries;
        size_t written_size;

        ContentWriteProgress() : entries(), written_size(0) {}
    };

    using OnStartWriteFunction = std::function<void(const ContentWriteProgress&)>;
    using OnContentWriteFunction = std::function<void(const ContentWriteProgress&)>;

    struct InstallableProgram {
        NcmContentMetaKey meta_key;
        NacpStruct nacp_data;
        std::string icon_path;

        inline constexpr NcmContentMetaType GetContentMetaType() const {
            return static_cast<NcmContentMetaType>(this->meta_key.type);
        }
    };

    class Installer {
        private:
            fs::PFS0 pfs0_file;
            u8 keygen;
            cnt::TicketFile tik_file;
            cnt::PackagedContentMeta packaged_cnt_meta;
            NcmStorageId storage_id;
            NcmContentStorage cnt_storage;
            NcmContentMetaDatabase cnt_meta_db;
            u64 base_app_id;
            u64 tik_file_size;
            std::string tik_file_name;
            u64 cert_file_size;
            std::string cert_file_name;
            NcmContentInfo meta_cnt_info;
            std::vector<NcmContentInfo> contents;
            std::vector<InstallableProgram> programs;

            Result StartProgramInstallation(const InstallableProgram &program);

        public:
            Installer(const std::string &path, fs::Explorer *exp, const NcmStorageId st_id) : pfs0_file(exp, path), storage_id(st_id), contents(), programs() {}
            ~Installer();
    
            Result PrepareInstallation();
            Result StartInstallation();

            inline constexpr bool HasTicket() {
                return this->tik_file_size > 0;
            }

            inline const cnt::TicketFile &GetTicketFile() {
                return this->tik_file;
            }

            inline constexpr u8 GetKeyGeneration() {
                return this->keygen;
            }

            inline std::vector<InstallableProgram> &GetPrograms() {
                return this->programs;
            }

            inline std::vector<NcmContentInfo> &GetContents() {
                return this->contents;
            }

            Result WriteContents(OnStartWriteFunction on_start_write_fn, OnContentWriteFunction on_content_write_fn);
            void FinalizeInstallation();
    };

}
