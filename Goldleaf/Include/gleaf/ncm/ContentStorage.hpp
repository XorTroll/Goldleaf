
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <gleaf/Types.hpp>
#include <gleaf/ncm/NCM.hpp>

namespace gleaf::ncm
{
    class ContentStorage
    {
        public: 
            ContentStorage(FsStorageId StorageId);
            ~ContentStorage();
            ContentStorage& operator=(const ContentStorage&) = delete;
            ContentStorage(const ContentStorage&) = delete;  
            Result CreatePlaceHolder(const NcmNcaId &PlaceHolderId, const NcmNcaId &RegisteredId, size_t Size);
            Result DeletePlaceHolder(const NcmNcaId &PlaceHolderId);
            Result WritePlaceHolder(const NcmNcaId &PlaceHolderId, u64 Offset, void *Buffer, size_t Size);
            Result Register(const NcmNcaId &PlaceHolderId, const NcmNcaId &RegisteredId);
            Result Delete(const NcmNcaId &RegisteredId);
            bool Has(const NcmNcaId &RegisteredId);
            std::string GetPath(const NcmNcaId &RegisteredId);
        private:
            NcmContentStorage storage;
    };
}