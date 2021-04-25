
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <fs/fs_FileSystem.hpp>

namespace dump {

    void DecryptCopyNAX0ToNCA(NcmContentStorage *cnt_storage, NcmContentId cnt_id, String path, std::function<void(double Done, double Total)> cb_fn);
    bool GetMetaRecord(NcmContentMetaDatabase *cnt_meta_db, u64 app_id, NcmContentMetaKey *out);
    NcmStorageId GetApplicationLocation(u64 app_id);
    void GenerateTicketCert(u64 app_id);
    String GetContentIdPath(NcmContentStorage *cnt_storage, NcmContentId *cnt_id);
    bool GetContentId(NcmContentMetaDatabase *cnt_meta_db, const NcmContentMetaKey *cnt_meta_key, u64 app_id, NcmContentType cnt_type, NcmContentId *out);

}