
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
#include <Types.hpp>

namespace amssu
{
    struct UpdateInformation
    {
        u32 version;
        bool exfat_supported;
        u32 fw_variation_count;

    };

    struct UpdateValidationInfo {
        Result result;
        Result exfat_result;
        NcmContentMetaKey invalid_key;
        NcmContentId invalid_content_id;
    };

    struct SystemUpdateProgress {
        s64 current_size;
        s64 total_size;
    };

    Result Initialize();
    void Exit();
    bool HasInitialized();

    Result GetUpdateInformation(const char *path, UpdateInformation *out_info);
    Result ValidateUpdate(const char *path, UpdateValidationInfo *out_info);
    Result SetupUpdate(void *buf, size_t size, const char *path, bool exfat);
    Result SetupUpdateWithVariation(void *buf, size_t size, const char *path, bool exfat, u32 variation);
    Result RequestPrepareUpdate(AsyncResult *async);
    Result GetPrepareUpdateProgress(NsSystemUpdateProgress *out);
    Result HasPreparedUpdate(bool *out_has);
    Result ApplyPreparedUpdate();
}