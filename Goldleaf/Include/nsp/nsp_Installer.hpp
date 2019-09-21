
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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
#include <es/es_Service.hpp>
#include <ns/ns_Service.hpp>
#include <fs/fs_Explorer.hpp>
#include <hos/hos_Content.hpp>
#include <hos/hos_Titles.hpp>
#include <ncm/ncm_Service.hpp>

namespace nsp
{
    class Installer
    {
        public:
            Installer(pu::String Path, fs::Explorer *Exp, Storage Location);
            ~Installer();
            Result PrepareInstallation();
            Result PreProcessContents();
            ncm::ContentMetaType GetContentMetaType();
            u64 GetApplicationId();
            std::string GetExportedIconPath();
            NacpStruct *GetNACP();
            bool HasTicket();
            hos::TicketData GetTicketData();
            u8 GetKeyGeneration();
            std::vector<ncm::ContentRecord> GetNCAs();
            Result WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total, u64 BytesSec)> OnContentWrite);
            void FinalizeInstallation();
        private:
            PFS0 nspentry;
            NacpStruct entrynacp;
            u8 keygen;
            hos::TicketData entrytik;
            ncm::ContentMeta cnmt;
            FsStorageId storage;
            ByteBuffer ccnmt;
            NcmMetaRecord mrec;
            u64 baseappid;
            u64 stik;
            pu::String tik;
            std::vector<ncm::ContentRecord> ncas;
            pu::String icon;
    };
}