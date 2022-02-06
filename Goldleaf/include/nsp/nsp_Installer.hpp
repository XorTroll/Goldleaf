
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
#include <nsp/nsp_PFS0.hpp>
#include <ncm/ncm_PackagedContentMeta.hpp>
#include <es/es_Service.hpp>
#include <ns/ns_Service.hpp>
#include <fs/fs_FileSystem.hpp>
#include <hos/hos_Content.hpp>
#include <hos/hos_Titles.hpp>

namespace nsp {

    class Installer {
        public:
            using OnContentsWriteFunction = std::function<void(const NcmContentInfo, const u32, const u32, const double, const double, const u64)>;

        private:
            PFS0 pfs0_file;
            NacpStruct nacp_data;
            u8 keygen;
            hos::TicketFile tik_file;
            ncm::PackagedContentMeta packaged_cnt_meta;
            NcmStorageId storage_id;
            NcmContentMetaKey cnt_meta_key;
            NcmContentStorage cnt_storage;
            NcmContentMetaDatabase cnt_meta_db;
            u64 base_app_id;
            u64 tik_file_size;
            std::string tik_file_name;
            NcmContentInfo meta_cnt_info;
            std::vector<NcmContentInfo> contents;
            std::string icon;

        public:
            Installer(const std::string &path, fs::Explorer *exp, const NcmStorageId st_id) : pfs0_file(exp, path), storage_id(st_id) {}
            ~Installer();
    
            Result PrepareInstallation();
            Result StartInstallation();
            
            inline constexpr NcmContentMetaType GetContentMetaType() {
                return static_cast<NcmContentMetaType>(this->cnt_meta_key.type);
            }
            
            inline constexpr u64 GetApplicationId() {
                return this->cnt_meta_key.id;
            }

            inline std::string GetExportedIconPath() {
                return this->icon;
            }
            
            inline const NacpStruct &GetNacp() {
                return this->nacp_data;
            }
            
            inline constexpr bool HasTicket() {
                return this->tik_file_size > 0;
            }

            inline const hos::TicketFile &GetTicketFile() {
                return this->tik_file;
            }

            inline constexpr u8 GetKeyGeneration() {
                return this->keygen;
            }

            inline std::vector<NcmContentInfo> &GetContents() {
                return this->contents;
            }

            Result WriteContents(OnContentsWriteFunction on_content_write_cb);
            void FinalizeInstallation();
    };

}