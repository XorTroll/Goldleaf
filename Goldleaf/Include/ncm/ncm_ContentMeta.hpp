
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <cstring>
#include <Types.hpp>
#include <ByteBuffer.hpp>
#include <ncm/ncm_Types.hpp>

namespace ncm
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