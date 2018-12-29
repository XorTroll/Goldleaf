
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <switch/arm/atomics.h>
#include <cstring>
#include <tuple>

namespace gleaf::ncm
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result CreatePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, const NcmNcaId *RegisteredId, u64 Size);
    Result DeletePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId);
    Result WritePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, u64 Offset, void *Buffer, size_t BufSize);
    Result Delete(NcmContentStorage *Storage, const NcmNcaId *RegisteredId);

    namespace ContentMetaDatabase
    {
        std::tuple<Result, u64> GetSize(NcmContentMetaDatabase *Database, const NcmMetaRecord *Record);
    }
}