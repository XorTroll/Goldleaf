
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
#include <gleaf/horizon/Title.hpp>
#include <gleaf/horizon/NCAID.hpp>
#include <gleaf/ncm/Content.hpp>
#include <gleaf/ncm/ContentMeta.hpp>
#include <gleaf/ncm/ContentStorage.hpp>

namespace gleaf::nsp
{
    class Installer
    {
        public:
            Installer(Destination Location, std::string Input, bool IgnoreVersion);
            ~Installer();
            void InitializeRecords();
            void InstallNCAs(std::function<void(ncm::ContentRecord, u32, u32, int)> OnChunk);
            void Finish();
            u64 GetApplicationId();
            ncm::ContentMetaType GetContentType();
            std::vector<ncm::ContentRecord> GetNCAList();
        private:
            FsStorageId stid;
            u64 basetid;
            std::string input;
            std::vector<ncm::ContentRecord> ncas;
            FsFileSystem idfs;
            ByteBuffer cnmtbuf;
            ncm::ContentMeta cmeta;
    };
}