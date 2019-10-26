
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

#pragma once
#include <cstring>
#include <Types.hpp>
#include <ByteBuffer.hpp>

namespace ncm
{
    struct PackagedContentInfo
    {
        u8 hash[0x20];
        NcmContentInfo content_info;
    } PACKED;

    struct PackagedContentMetaHeader
    {
        u64 title_id;
        u32 version;
        u8 type;
        u8 _0xd;
        u16 extended_header_size;
        u16 content_count;
        u16 content_meta_count;
        u8 attributes;
        u8 storage_id;
        u8 install_type;
        bool comitted;
        u32 required_system_version;
        u32 _0x1c;
    };

    class ContentMeta
    {
        public:
            ContentMeta();
            ContentMeta(u8 *Data, size_t Size);
            ~ContentMeta();
            PackagedContentMetaHeader GetPackagedContentMetaHeader();
            NcmContentMetaKey GetContentMetaKey();
            std::vector<NcmContentInfo> GetContentInfos();
            void GetInstallContentMeta(ByteBuffer &CNMTBuffer, NcmContentInfo &CNMTRecord, bool IgnoreVersion);
        private:
            ByteBuffer buf;
    };
}