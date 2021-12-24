
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

#include <ncm/ncm_PackagedContentMeta.hpp>

namespace ncm {

    void PackagedContentMeta::CreateContentMetaForInstall(const NcmContentInfo self_cnt_info, u8 *&out_data, size_t &out_data_size, const bool ignore_required_fw_ver) {
        const auto content_count_with_self = this->contents.size() + 1;
        auto size = sizeof(NcmContentMetaHeader) + this->header.header.extended_header_size + content_count_with_self * sizeof(NcmContentInfo);
        const auto cnt_type = static_cast<NcmContentMetaType>(this->header.type);
        switch(cnt_type) {
            case NcmContentMetaType_SystemUpdate: {
                size += this->extended_header.system_update.extended_data_size;
                break;
            }
            case NcmContentMetaType_Patch: {
                size += this->extended_header.patch.extended_data_size;
                break;
            }
            case NcmContentMetaType_Delta: {
                size += this->extended_header.delta.extended_data_size;
                break;
            }
            default: {
                break;
            }
        }

        auto meta_data = new u8[size]();

        auto cnt_meta_header = this->header.header;
        cnt_meta_header.content_count = static_cast<u16>(content_count_with_self);
        *reinterpret_cast<NcmContentMetaHeader*>(meta_data) = cnt_meta_header;

        auto meta_data_extended_header = meta_data + sizeof(NcmContentMetaHeader);
        switch(cnt_type) {
            case NcmContentMetaType_SystemUpdate: {
                *reinterpret_cast<NcmSystemUpdateMetaExtendedHeader*>(meta_data_extended_header) = this->extended_header.system_update;
                break;
            }
            case NcmContentMetaType_Application: {
                auto extended_header = this->extended_header.application;
                if(ignore_required_fw_ver) {
                    extended_header.required_system_version = 0;
                }
                *reinterpret_cast<NcmApplicationMetaExtendedHeader*>(meta_data_extended_header) = extended_header;
                break;
            }
            case NcmContentMetaType_Patch: {
                auto extended_header = this->extended_header.patch;
                if(ignore_required_fw_ver) {
                    extended_header.required_system_version = 0;
                }
                *reinterpret_cast<NcmPatchMetaExtendedHeader*>(meta_data_extended_header) = extended_header;
                break;
            }
            case NcmContentMetaType_AddOnContent: {
                *reinterpret_cast<NcmAddOnContentMetaExtendedHeader*>(meta_data_extended_header) = this->extended_header.aoc;
                break;
            }
            case NcmContentMetaType_Delta: {
                *reinterpret_cast<DeltaMetaExtendedHeader*>(meta_data_extended_header) = this->extended_header.delta;
                break;
            }
            default: {
                break;
            }
        }

        auto meta_data_cnt_info = reinterpret_cast<NcmContentInfo*>(meta_data_extended_header + this->header.header.extended_header_size);

        *meta_data_cnt_info = self_cnt_info;
        meta_data_cnt_info++;

        for(const auto &cnt: this->contents) {
            *meta_data_cnt_info = cnt.info;
            meta_data_cnt_info++;
        }

        out_data = meta_data;
        out_data_size = size;
    }

    bool ReadContentMeta(const u8 *cnmt_buf, const size_t cnmt_size, PackagedContentMeta &out_cnmt) {
        const auto header = *reinterpret_cast<const PackagedContentMetaHeader*>(cnmt_buf);
        out_cnmt.header = header;

        const auto cnt_type = static_cast<NcmContentMetaType>(header.type);
        auto extended_header = cnmt_buf + sizeof(PackagedContentMetaHeader);
        switch(cnt_type) {
            case NcmContentMetaType_SystemUpdate: {
                if(header.header.extended_header_size != sizeof(NcmSystemUpdateMetaExtendedHeader)) {
                    return false;
                }
                out_cnmt.extended_header.system_update = *reinterpret_cast<const NcmSystemUpdateMetaExtendedHeader*>(extended_header);
                break;
            }
            case NcmContentMetaType_Application: {
                if(header.header.extended_header_size != sizeof(NcmApplicationMetaExtendedHeader)) {
                    return false;
                }
                out_cnmt.extended_header.application = *reinterpret_cast<const NcmApplicationMetaExtendedHeader*>(extended_header);
                break;
            }
            case NcmContentMetaType_Patch: {
                if(header.header.extended_header_size != sizeof(NcmPatchMetaExtendedHeader)) {
                    return false;
                }
                out_cnmt.extended_header.patch = *reinterpret_cast<const NcmPatchMetaExtendedHeader*>(extended_header);
                break;
            }
            case NcmContentMetaType_AddOnContent: {
                if(header.header.extended_header_size != sizeof(NcmAddOnContentMetaExtendedHeader)) {
                    return false;
                }
                out_cnmt.extended_header.aoc = *reinterpret_cast<const NcmAddOnContentMetaExtendedHeader*>(extended_header);
                break;
            }
            case NcmContentMetaType_Delta: {
                if(header.header.extended_header_size != sizeof(DeltaMetaExtendedHeader)) {
                    return false;
                }
                out_cnmt.extended_header.delta = *reinterpret_cast<const DeltaMetaExtendedHeader*>(extended_header);
                break;
            }
            default: {
                return false;
            }
        }

        out_cnmt.contents.clear();
        for(u32 i = 0; i < header.header.content_count; i++) {
            const auto content_info_offset = sizeof(PackagedContentMetaHeader) + header.header.extended_header_size + i * sizeof(NcmPackagedContentInfo);
            const auto content_info = *reinterpret_cast<const NcmPackagedContentInfo*>(cnmt_buf + content_info_offset);

            // Delta fragments are not like the other installable content
            if(static_cast<NcmContentType>(content_info.info.content_type) != NcmContentType_DeltaFragment) {
                out_cnmt.contents.push_back(content_info);
            }
        }

        return true;
    }

}