
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <gleaf/nsp/PFS0.hpp>
#include <gleaf/es/ES.hpp>
#include <gleaf/ns/NS.hpp>
#include <gleaf/fs/FS.hpp>
#include <gleaf/horizon.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf::nsp
{
    class Installer
    {
        public:
            Installer(std::string Path, fs::Explorer *Exp, Storage Location);
            ~Installer();
            Result PrepareInstallation();
            Result PreProcessContents();
            ncm::ContentMetaType GetContentMetaType();
            u64 GetApplicationId();
            std::string GetExportedIconPath();
            NacpStruct *GetNACP();
            bool HasTicket();
            horizon::TicketData GetTicketData();
            std::vector<ncm::ContentRecord> GetNCAs();
            Result WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total, u64 BytesSec)> OnContentWrite);
            void FinalizeInstallation();
        private:
            PFS0 nspentry;
            NacpStruct *entrynacp;
            horizon::TicketData entrytik;
            ncm::ContentMeta *cnmt;
            FsStorageId storage;
            ByteBuffer ccnmt;
            ncm::ContentRecord record;
            NcmMetaRecord mrec;
            u64 baseappid;
            u64 stik;
            std::string tik;
            std::vector<ncm::ContentRecord> ncas;
            std::string icon;
    };
}