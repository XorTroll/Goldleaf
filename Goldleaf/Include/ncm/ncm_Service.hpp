
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

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