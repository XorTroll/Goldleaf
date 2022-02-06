
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

    using DecryptCallback = std::function<void(const double, const double)>;

    void DecryptCopyNAX0ToNCA(NcmContentStorage *cnt_storage, const NcmContentId cnt_id, const std::string &path, DecryptCallback cb_fn);
    bool GetMetaRecord(NcmContentMetaDatabase *cnt_meta_db, const u64 app_id, NcmContentMetaKey *out);
    NcmStorageId GetApplicationLocation(const u64 app_id);
    void GenerateTicketCert(const u64 app_id);
    std::string GetContentIdPath(NcmContentStorage *cnt_storage, const NcmContentId cnt_id);
    bool GetContentId(NcmContentMetaDatabase *cnt_meta_db, const NcmContentMetaKey *cnt_meta_key, const u64 app_id, const NcmContentType cnt_type, NcmContentId *out);

}