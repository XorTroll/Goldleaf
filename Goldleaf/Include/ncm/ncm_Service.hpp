
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <ncm/ncm_Types.hpp>
#include <ncm/ncm_ContentMeta.hpp>

namespace ncm
{
    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result CreatePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, const NcmNcaId *RegisteredId, u64 Size);
    Result DeletePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId);
    Result WritePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, u64 Offset, void *Buffer, size_t BufSize);
    Result Delete(NcmContentStorage *Storage, const NcmNcaId *RegisteredId);

    Result ContentMetaDatabase_GetSize(NcmContentMetaDatabase *Database, const NcmMetaRecord *Record, u64 *out_Size);
}