
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

#pragma once
#include <ns/ns_Types.hpp>

namespace ns {

    Result PushApplicationRecord(u64 app_id, u8 last_modified_event, ContentStorageMetaKey *cnt_storage_record_buf, size_t cnt_storage_record_buf_count);
    Result ListApplicationRecordContentMeta(u64 offset, u64 app_id, ContentStorageMetaKey *out_buf, size_t out_buf_count, u32 *out_count);
    Result DeleteApplicationRecord(u64 app_id);
    Result PushLaunchVersion(u64 app_id, u32 launch_version);

}