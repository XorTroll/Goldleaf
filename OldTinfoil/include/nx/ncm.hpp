#pragma once

#include <string>

extern "C"
{
#include <switch/services/fs.h>
#include <switch/services/ncm.h>
}

#include "nx/ipc/tin_ipc.h"

namespace nx::ncm
{
    class ContentStorage final
    {
        private:
            NcmContentStorage m_contentStorage;

        public:
            // Don't allow copying, or garbage may be closed by the destructor
            ContentStorage& operator=(const ContentStorage&) = delete;
            ContentStorage(const ContentStorage&) = delete;   

            ContentStorage(FsStorageId storageId);
            ~ContentStorage();

            void CreatePlaceholder(const NcmNcaId &placeholderId, const NcmNcaId &registeredId, size_t size);
            void DeletePlaceholder(const NcmNcaId &placeholderId);
            void WritePlaceholder(const NcmNcaId &placeholderId, u64 offset, void *buffer, size_t bufSize);
            void Register(const NcmNcaId &placeholderId, const NcmNcaId &registeredId);
            void Delete(const NcmNcaId &registeredId);
            bool Has(const NcmNcaId &registeredId);
            std::string GetPath(const NcmNcaId &registeredId);
    };
}