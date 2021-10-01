
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
#include <ncm/ncm_Types.hpp>

namespace ncm {

    struct PackagedContentMeta {
        PackagedContentMetaHeader header;
        std::vector<NcmPackagedContentInfo> contents;

        union {
            NcmSystemUpdateMetaExtendedHeader system_update;
            NcmApplicationMetaExtendedHeader application;
            NcmPatchMetaExtendedHeader patch;
            NcmAddOnContentMetaExtendedHeader aoc;
            DeltaMetaExtendedHeader delta;
        } extended_header;

        void CreateContentMetaForInstall(const NcmContentInfo self_cnt_info, u8 *&out_data, size_t &out_data_size, const bool ignore_required_fw_ver);
    };

    bool ReadContentMeta(const u8 *cnmt_buf, const size_t cnmt_size, PackagedContentMeta &out_cnmt);

}