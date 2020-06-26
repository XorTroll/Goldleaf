
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <Types.hpp>
#include <nsp/nsp_PFS0.hpp>
#include <ncm/ncm_ContentMeta.hpp>
#include <es/es_Service.hpp>
#include <ns/ns_Service.hpp>
#include <fs/fs_FileSystem.hpp>
#include <hos/hos_Content.hpp>
#include <hos/hos_Titles.hpp>

namespace nsp
{
    using OnContentsWriteFunction = std::function<void(ncm::ContentRecord, u32, u32, double, double, u64)>;

    class Installer
    {
        private:
            PFS0 pfs0_file;
            NacpStruct nacp_data;
            u8 keygen;
            hos::TicketFile tik_file;
            ncm::ContentMeta cnmt;
            NcmStorageId storage_id;
            ByteBuffer cnmt_buf;
            NcmContentMetaKey cnt_meta_key;
            NcmContentStorage cnt_storage;
            NcmContentMetaDatabase cnt_meta_db;
            u64 base_app_id;
            u64 tik_file_size;
            String tik_file_name;
            std::vector<ncm::ContentRecord> ncas;
            String icon;

        public:
            Installer(String Path, fs::Explorer *Exp, Storage Location) : pfs0_file(Exp, Path), storage_id(static_cast<NcmStorageId>(Location)) {}
            ~Installer();
    
            Result PrepareInstallation();
            Result PreProcessContents();
            ncm::ContentMetaType GetContentMetaType();
            u64 GetApplicationId();
            std::string GetExportedIconPath();
            NacpStruct *GetNACP();
            bool HasTicket();
            hos::TicketFile GetTicketFile();
            u8 GetKeyGeneration();
            std::vector<ncm::ContentRecord> GetNCAs();
            Result WriteContents(OnContentsWriteFunction OnContentWrite);
            void FinalizeInstallation();
        
    };
}