
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

#include <fs/fs_Explorer.hpp>

namespace dump
{
    enum class NCAType
    {
        Program,
        Meta,
        Control,
        LegalInfo,
        OfflineHtml,
        Data,
    };

    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmNcaId NCAId, pu::String Path, std::function<void(double Done, double Total)> Callback);
    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmMetaRecord *out);
    FsStorageId GetApplicationLocation(u64 ApplicationId);
    void GenerateTicketCert(u64 ApplicationId);
    pu::String GetNCAIdPath(NcmContentStorage *st, NcmNcaId *Id);
    bool GetNCAId(NcmContentMetaDatabase *cmdb, NcmMetaRecord *rec, u64 ApplicationId, NCAType Type, NcmNcaId *out);
}