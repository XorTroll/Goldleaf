
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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
#include <hos/hos_Titles.hpp>

namespace expt {

    using DecryptStartCallback = std::function<void(const double)>;
    using DecryptProgressCallback = std::function<void(const double)>;

    Result DecryptCopyNAX0ToNCA(NcmContentStorage *cnt_storage, const NcmContentId cnt_id, const std::string &path, DecryptStartCallback dec_start_cb, DecryptProgressCallback dec_prog_cb);
    std::string ExportTicketCert(const u64 app_id, const bool export_cert);
    std::string GetContentIdPath(NcmContentStorage *cnt_storage, const NcmContentId cnt_id);

}