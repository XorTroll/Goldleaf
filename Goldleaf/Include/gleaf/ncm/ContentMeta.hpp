
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <cstring>
#include <gleaf/Types.hpp>
#include <gleaf/ByteBuffer.hpp>
#include <gleaf/ncm/Content.hpp>

namespace gleaf::ncm
{
    class ContentMeta
    {
        public:
            ContentMeta();
            ContentMeta(u8 *Data, size_t Size);
            ~ContentMeta();
            ContentMetaHeader GetContentMetaHeader();
            NcmMetaRecord GetContentMetaKey();
            std::vector<ContentRecord> GetContentRecords();
            void GetInstallContentMeta(ByteBuffer &CNMTBuffer, ContentRecord &CNMTRecord, bool IgnoreVersion);
        private:
            ByteBuffer buf;
    };
}