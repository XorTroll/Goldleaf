#include <gleaf/ncm/ContentStorage.hpp>

namespace gleaf::ncm
{
    ContentStorage::ContentStorage(FsStorageId StorageId)
    {
        NcmContentStorage st;
        Result rc = ncmOpenContentStorage(StorageId, &st);
        if(rc != 0) ThrowError("Failed to open content storage: " + std::to_string(rc));
        this->storage = st;
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
        Result rc = ncmContentStorageHas(&this->storage, &RegisteredId, &has);
        if(rc != 0) ThrowError("Failed to get whether the storage contains a NCA Id.");
        return has;
    }

    std::string ContentStorage::GetPath(const NcmNcaId &RegisteredId)
    {
        char npath[FS_MAX_PATH] = { 0 };
        Result rc = ncmContentStorageGetPath(&this->storage, &RegisteredId, npath, FS_MAX_PATH);
        if(rc != 0) ThrowError("Failed to get NCA Id path.");
        return std::string(npath);
    }
}