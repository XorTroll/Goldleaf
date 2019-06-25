#include <ncm/ncm_ContentStorage.hpp>

namespace ncm
{
    ContentStorage::ContentStorage(FsStorageId StorageId)
    {
        ncmOpenContentStorage(StorageId, &this->storage);
    }

    ContentStorage::~ContentStorage()
    {
        serviceClose(&this->storage.s);
    }

    Result ContentStorage::CreatePlaceHolder(const NcmNcaId &PlaceHolderId, const NcmNcaId &RegisteredId, size_t Size)
    {
        return ncm::CreatePlaceHolder(&this->storage, &PlaceHolderId, &RegisteredId, Size);
    }

    Result ContentStorage::DeletePlaceHolder(const NcmNcaId &PlaceHolderId)
    {
        return ncm::DeletePlaceHolder(&this->storage, &PlaceHolderId);
    }

    Result ContentStorage::WritePlaceHolder(const NcmNcaId &PlaceHolderId, u64 Offset, void *Buffer, size_t Size)
    {
        return ncm::WritePlaceHolder(&this->storage, &PlaceHolderId, Offset, Buffer, Size);
    }

    Result ContentStorage::Register(const NcmNcaId &PlaceHolderId, const NcmNcaId &RegisteredId)
    {
        return ncmContentStorageRegister(&this->storage, &RegisteredId, &PlaceHolderId);
    }

    Result ContentStorage::Delete(const NcmNcaId &RegisteredId)
    {
        return ncm::Delete(&this->storage, &RegisteredId);
    }

    bool ContentStorage::Has(const NcmNcaId &RegisteredId)
    {
        bool has = false;
        ncmContentStorageHas(&this->storage, &RegisteredId, &has);
        return has;
    }

    std::string ContentStorage::GetPath(const NcmNcaId &RegisteredId)
    {
        char npath[FS_MAX_PATH] = { 0 };
        ncmContentStorageGetPath(&this->storage, &RegisteredId, npath, FS_MAX_PATH);
        return std::string(npath);
    }
}