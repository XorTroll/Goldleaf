
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

#include <ncm/ncm_ContentMeta.hpp>

namespace ncm {

    ContentMeta::ContentMeta() {
        this->buf.Resize(sizeof(ContentMetaHeader));
    }

    ContentMeta::ContentMeta(const u8* data, size_t size) : buf(size) {
        this->buf.Resize(size);
        memcpy(this->buf.GetData(), data, size);
    }

    ContentMetaHeader ContentMeta::GetContentMetaHeader() {
        return this->buf.Read<ContentMetaHeader>(0);
    }

    NcmContentMetaKey ContentMeta::GetContentMetaKey() {
        const auto cnt_meta_header = this->GetContentMetaHeader();

        const NcmContentMetaKey meta_key = {
            .id = cnt_meta_header.app_id,
            .version = cnt_meta_header.title_version,
            .type = cnt_meta_header.type,
        };
        return meta_key;
    }

    std::vector<NcmContentInfo> ContentMeta::GetContents() {
        const auto cnt_meta_header = this->GetContentMetaHeader();
        std::vector<NcmContentInfo> contents;
        auto packaged_cnt_infos = reinterpret_cast<NcmPackagedContentInfo*>(buf.GetData() + sizeof(ContentMetaHeader) + cnt_meta_header.extended_header_size);
        for(u32 i = 0; i < cnt_meta_header.content_count; i++) {
            const auto cur_packaged_cnt_info = packaged_cnt_infos[i];
            if(cur_packaged_cnt_info.info.content_type != NcmContentType_DeltaFragment) {
                contents.push_back(cur_packaged_cnt_info.info); 
            }
        }
        return contents;
    }

    void ContentMeta::GetInstallContentMeta(ByteBuffer &cnmt_buf, NcmContentInfo &cnt_info, bool ignore_ver) {
        const auto cnt_meta_header = this->GetContentMetaHeader();
        const auto contents = this->GetContents();
        const NcmContentMetaHeader install_cnt_meta_header = {
            .extended_header_size = cnt_meta_header.extended_header_size,
            .content_count = static_cast<u16>(contents.size() + 1),
            .content_meta_count = cnt_meta_header.content_meta_count,
            .attributes = cnt_meta_header.attributes; // Sparse Titles use 0x04 not 0x0
        };
        cnmt_buf.Append(install_cnt_meta_header);
        cnmt_buf.Resize(cnmt_buf.GetSize() + install_cnt_meta_header.extended_header_size);
        auto extended_header_src_bytes = buf.GetData() + sizeof(ContentMetaHeader);
        auto install_extended_header_start = cnmt_buf.GetData() + sizeof(NcmContentMetaHeader);
        memcpy(install_extended_header_start, extended_header_src_bytes, install_cnt_meta_header.extended_header_size);
        if(ignore_ver && ((static_cast<NcmContentMetaType>(cnt_meta_header.type) == NcmContentMetaType_Application) || (static_cast<NcmContentMetaType>(cnt_meta_header.type) == NcmContentMetaType_Patch))) {
            cnmt_buf.Write(static_cast<u32>(0), sizeof(NcmContentMetaHeader) + 8);
        }
        cnmt_buf.Append(cnt_info);
        for(const auto &cnt: contents) {
            cnmt_buf.Append(cnt);
        }
        if(static_cast<NcmContentMetaType>(cnt_meta_header.type) == NcmContentMetaType_Patch) {
            auto patch_meta_extended_header = reinterpret_cast<NcmPatchMetaExtendedHeader*>(extended_header_src_bytes);
            cnmt_buf.Resize(cnmt_buf.GetSize() + patch_meta_extended_header->extended_data_size);
        }
    }

}
