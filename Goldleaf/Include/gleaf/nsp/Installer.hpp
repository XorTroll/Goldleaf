
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <gleaf/Types.hpp>
#include <gleaf/es/ES.hpp>
#include <gleaf/ns/NS.hpp>
#include <gleaf/fs/FS.hpp>
#include <gleaf/horizon.hpp>
#include <gleaf/ncm.hpp>

namespace gleaf::nsp
{
    enum class InstallerError
    {
        Success,
        BadNSP,
        NSPOpen,
        BadCNMTNCA,
        CNMTMCAOpen,
        BadCNMT,
        CNMTOpen,
        BadControlNCA,
        MetaDatabaseOpen,
        MetaDatabaseSet,
        MetaDatabaseCommit,
        ContentMetaCount,
        ContentMetaList,
        RecordPush,
        InstallBadNCA,
    };

    struct InstallerResult
    {
        Result Error;
        InstallerError Type;

        bool IsSuccess();
    };

    class Installer
    {
        public:
            Installer(Destination Location, std::string Input, bool IgnoreVersion);
            ~Installer();
            InstallerResult ProcessRecords();
            InstallerResult WriteContents(std::function<void(ncm::ContentRecord Record, u32 Content, u32 ContentCount, int Percentage)> Callback);
            NacpStruct *GetNACP();
            u64 GetApplicationId();
            ncm::ContentMetaType GetContentType();
            std::vector<ncm::ContentRecord> GetRecords();
            std::string GetExportedIconPath();
            bool HasContent(ncm::ContentType Type);
            bool HasTicketAndCert();
            horizon::TicketData GetTicketData();
            bool IsCNMTAlreadyInstalled();
            InstallerResult GetLatestResult();
            void Finalize();
        private:
            bool icnmt;
            bool tik;
            std::string icn;
            NacpStruct *nacps;
            InstallerResult irc;
            FsStorageId stid;
            u64 basetid;
            std::vector<ncm::ContentRecord> ncas;
            FsFileSystem idfs;
            ByteBuffer cnmtbuf;
            ncm::ContentMeta cmeta;
            std::string stik;
            std::string scert;
            horizon::TicketData tikdata;
    };
}