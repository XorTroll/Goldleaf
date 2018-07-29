#pragma once

#include <switch.h>

#include "nx/ipc/tin_ipc.h"

namespace nx::ncm
{
    class ContentStorage final
    {
        private:
            NcmContentStorage m_contentStorage;

        public:
            ContentStorage(FsStorageId storageId);
            ~ContentStorage();

            void CreatePlaceholder(const NcmNcaId &placeholderId, const NcmNcaId &registeredId, size_t size);
            void DeletePlaceholder(const NcmNcaId &placeholderId);
            void WritePlaceholder(const NcmNcaId &placeholderId, u64 offset, void *buffer, size_t bufSize);
            void Register(const NcmNcaId &placeholderId, const NcmNcaId &registeredId);
            void Delete(const NcmNcaId &registeredId);
    };
}