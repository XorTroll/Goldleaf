
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

#include <ncm/ncm_ContentMeta.hpp>

namespace ncm
{
    ContentMeta::ContentMeta()
    {
        buf.Resize(sizeof(PackagedContentMetaHeader));
    }

    ContentMeta::ContentMeta(u8* Data, size_t Size) : buf(Size)
    {
        buf.Resize(Size);
        memcpy(buf.GetData(), Data, Size);
    }

    ContentMeta::~ContentMeta()
    {
    }

    PackagedContentMetaHeader ContentMeta::GetPackagedContentMetaHeader()
    {
        return buf.Read<PackagedContentMetaHeader>(0);
    }

    NcmContentMetaKey ContentMeta::GetContentMetaKey()
    {
        NcmContentMetaKey metaRecord;
        PackagedContentMetaHeader contentMetaHeader = this->GetPackagedContentMetaHeader();
        memset(&metaRecord, 0, sizeof(NcmContentMetaKey));
        metaRecord.title_id = contentMetaHeader.title_id;
        metaRecord.version = contentMetaHeader.version;
        metaRecord.type = static_cast<NcmContentMetaType>(contentMetaHeader.type);
        return metaRecord;
    }

    std::vector<NcmContentInfo> ContentMeta::GetContentInfos()
    {
        PackagedContentMetaHeader contentMetaHeader = this->GetPackagedContentMetaHeader();
        std::vector<NcmContentInfo> contentInfos;
        PackagedContentInfo* packagedContentInfos = (PackagedContentInfo*)(buf.GetData() + sizeof(PackagedContentMetaHeader) + contentMetaHeader.extended_header_size);
        for (unsigned int i = 0; i < contentMetaHeader.content_count; i++)
        {
            PackagedContentInfo packagedContentInfo = packagedContentInfos[i];
            if (static_cast<u8>(packagedContentInfo.content_info.content_type) <= 5) contentInfos.push_back(packagedContentInfo.content_info);
        }

        return contentInfos;
    }

    void ContentMeta::GetInstallContentMeta(ByteBuffer &installContentMetaBuffer, NcmContentInfo &cnmtNcmContentInfo, bool ignoreReqFirmVersion)
    {
        PackagedContentMetaHeader packagedContentMetaHeader = this->GetPackagedContentMetaHeader();
        std::vector<NcmContentInfo> contentInfos = this->GetContentInfos();
        NcmContentMetaHeader contentMetaHeader;
        contentMetaHeader.extended_header_size = packagedContentMetaHeader.extended_header_size;
        contentMetaHeader.content_count = contentInfos.size() + 1;
        contentMetaHeader.content_meta_count = packagedContentMetaHeader.content_meta_count;
        installContentMetaBuffer.Append<NcmContentMetaHeader>(contentMetaHeader);
        installContentMetaBuffer.Resize(installContentMetaBuffer.GetSize() + contentMetaHeader.extended_header_size);
        auto* extendedHeaderSourceBytes = buf.GetData() + sizeof(PackagedContentMetaHeader);
        u8* installExtendedHeaderStart = installContentMetaBuffer.GetData() + sizeof(NcmContentMetaHeader);
        memcpy(installExtendedHeaderStart, extendedHeaderSourceBytes, contentMetaHeader.extended_header_size);
        if (ignoreReqFirmVersion && (packagedContentMetaHeader.type == NcmContentMetaType_Application || packagedContentMetaHeader.type == NcmContentMetaType_Patch))
            installContentMetaBuffer.Write<u32>(0, sizeof(NcmContentMetaHeader) + 8);
        installContentMetaBuffer.Append<NcmContentInfo>(cnmtNcmContentInfo);
        for (auto& contentInfo : contentInfos)
            installContentMetaBuffer.Append<NcmContentInfo>(contentInfo);
        if (packagedContentMetaHeader.type == NcmContentMetaType_Patch)
        {
            NcmPatchMetaExtendedHeader* patchMetaExtendedHeader = (NcmPatchMetaExtendedHeader*)extendedHeaderSourceBytes;
            installContentMetaBuffer.Resize(installContentMetaBuffer.GetSize() + patchMetaExtendedHeader->extended_data_size);
        }
    }
}