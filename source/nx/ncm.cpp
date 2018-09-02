#include "nx/ncm.hpp"
#include "error.hpp"

namespace nx::ncm
{
    ContentStorage::ContentStorage(FsStorageId storageId) 
    {
        ASSERT_OK(ncmOpenContentStorage(storageId, &m_contentStorage), "Failed to open NCM ContentStorage");
    }

    ContentStorage::~ContentStorage()
    {
        serviceClose(&m_contentStorage.s);
    }

    void ContentStorage::CreatePlaceholder(const NcmNcaId &placeholderId, const NcmNcaId &registeredId, size_t size)
    {
        ASSERT_OK(ncmCreatePlaceHolder(&m_contentStorage, &placeholderId, &registeredId, size), "Failed to create placeholder");
    }
            
    void ContentStorage::DeletePlaceholder(const NcmNcaId &placeholderId)
    {
        ASSERT_OK(ncmDeletePlaceHolder(&m_contentStorage, &placeholderId), "Failed to delete placeholder");
    }

    void ContentStorage::WritePlaceholder(const NcmNcaId &placeholderId, u64 offset, void *buffer, size_t bufSize)
    {
        ASSERT_OK(ncmWritePlaceHolder(&m_contentStorage, &placeholderId, offset, buffer, bufSize), "Failed to write to placeholder");
    }

    void ContentStorage::Register(const NcmNcaId &placeholderId, const NcmNcaId &registeredId)
    {
        ASSERT_OK(ncmContentStorageRegister(&m_contentStorage, &registeredId, &placeholderId), "Failed to register placeholder NCA");
    }

    void ContentStorage::Delete(const NcmNcaId &registeredId)
    {
        ASSERT_OK(ncmDelete(&m_contentStorage, &registeredId), "Failed to delete registered NCA");
    }

    bool ContentStorage::Has(const NcmNcaId &registeredId)
    {
        bool hasNCA = false;
        ASSERT_OK(ncmContentStorageHas(&m_contentStorage, &registeredId, &hasNCA), "Failed to check if NCA is present");
        return hasNCA;
    }

    std::string ContentStorage::GetPath(const NcmNcaId &registeredId)
    {
        char pathBuf[FS_MAX_PATH] = {0};
        ASSERT_OK(ncmContentStorageGetPath(&m_contentStorage, &registeredId, pathBuf, FS_MAX_PATH), "Failed to get installed NCA path");
        return std::string(pathBuf);
    }
}