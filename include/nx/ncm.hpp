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
            ContentStorage();
            ~ContentStorage();

            Result Open(FsStorageId storageId);
            Result CreatePlaceholder(const NcmNcaId &placeholderId, const NcmNcaId &registeredId, size_t size);
            Result DeletePlaceholder(const NcmNcaId &placeholderId);
            Result WritePlaceholder(const NcmNcaId &placeholderId, u64 offset, void *buffer, size_t bufSize);
            Result Register(const NcmNcaId &placeholderId, const NcmNcaId &registeredId);
            Result Delete(const NcmNcaId &registeredId);
    };
}