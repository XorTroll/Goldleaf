#include "nx/ncm.hpp"

namespace nx::ncm
{
    ContentStorage::ContentStorage() {}

    ContentStorage::~ContentStorage()
    {
        serviceClose(&m_contentStorage.s);
    }

    Result ContentStorage::Open(FsStorageId storageId)
    {
        return ncmOpenContentStorage(storageId, &m_contentStorage);
    }

    Result ContentStorage::CreatePlaceholder(const NcmNcaId &placeholderId, const NcmNcaId &registeredId, size_t size)
    {
        return ncmCreatePlaceHolder(&m_contentStorage, &placeholderId, &registeredId, size);
    }
            
    Result ContentStorage::DeletePlaceholder(const NcmNcaId &placeholderId)
    {
        return ncmDeletePlaceHolder(&m_contentStorage, &placeholderId);
    }

    Result ContentStorage::WritePlaceholder(const NcmNcaId &placeholderId, u64 offset, void *buffer, size_t bufSize)
    {
        return ncmWritePlaceHolder(&m_contentStorage, &placeholderId, offset, buffer, bufSize);
    }

    Result ContentStorage::Register(const NcmNcaId &placeholderId, const NcmNcaId &registeredId)
    {
        return ncmContentStorageRegister(&m_contentStorage, &placeholderId, &registeredId);
    }

    Result ContentStorage::Delete(const NcmNcaId &registeredId)
    {
        return ncmDelete(&m_contentStorage, &registeredId);
    }
}