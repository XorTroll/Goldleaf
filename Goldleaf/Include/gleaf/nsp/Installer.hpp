
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
            Installer(Storage Location, std::string Input, bool IgnoreVersion);
            ~Installer();
            Result ProcessRecords();
            Result WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, int Percentage)> Callback);
            NacpStruct *GetNACP();
            u64 GetApplicationId();
            ncm::ContentMetaType GetContentType();
            std::vector<ncm::ContentRecord> GetRecords();
            std::string GetExportedIconPath();
            bool HasContent(ncm::ContentType Type);
            bool HasTicket();
            horizon::TicketData GetTicketData();
            bool IsCNMTAlreadyInstalled();
            Result GetLatestResult();
            void Finalize();
        private:
            bool icnmt;
            bool tik;
            std::string icn;
            NacpStruct *nacps;
            Result rc;
            FsStorageId stid;
            u64 basetid;
            std::vector<ncm::ContentRecord> ncas;
            FsFileSystem idfs;
            FsFileSystem cnmtfs;
            ByteBuffer cnmtbuf;
            ncm::ContentMeta cmeta;
            std::string stik;
            horizon::TicketData tikdata;
    };

    Result Install(std::string Path, fs::Explorer *Exp, Storage Location, std::function<bool(ncm::ContentMetaType Type, u64 ApplicationId, std::string IconPath, NacpStruct *NACP, horizon::TicketData *Tik, std::vector<ncm::ContentRecord> NCAs)> OnInitialProcess, std::function<void()> OnRecordProcess, std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total)> OnContentWrite);
}