#pragma once

#include <switch.h>
#include <vector>
#include "nx/ipc/tin_ipc.h"

namespace tin::install
{
    enum class ContentMetaType : u8
    {
        SYSTEM_PROGRAM          = 0x1,
        SYSTEM_DATA             = 0x2,
        SYSTEM_UPDATE           = 0x3,
        BOOT_IMAGE_PACKAGE      = 0x4,
        BOOT_IMAGE_PACKAGE_SAFE = 0x5,
        APPLICATION             = 0x80,
        PATCH                   = 0x81,
        ADD_ON_CONTENT          = 0x82,
        DELTA                   = 0x83
    };

    struct ContentMetaHeader
    {
        u64 titleId;
        u32 titleVersion;
        ContentMetaType type;
        u8 padding0;
        u16 extendedHeaderSize;
        u16 contentCount;
        u16 contentMetaCount;
        u8 attributes;
        u8 padding1[0x3];
        u32 requiredDownloadSystemVersion;
        u8 padding2[0x4];
    } PACKED;

    static_assert(sizeof(ContentMetaHeader) == 0x20, "ContentMetaHeader must be 0x20!");

    struct ApplicationMetaExtendedHeader
    {
        u64 patchTitleId;
        u32 requiredSystemVersion;
        u32 padding;
    } PACKED;

    struct PatchMetaExtendedHeader
    {
        u64 applicationTitleId;
        u32 requiredSystemVersion;
        u32 extendedDataSize;
        u8 padding[0x8];
    } PACKED;

    struct AddOnContentMetaExtendedHeader
    {
        u64 applicationTitleId;
        u32 requiredApplicationVersion;
        u32 padding;
    } PACKED;

    struct ContentRecord
    {
        NcmNcaId ncaId;
        u8 size[0x6];
        u8 contentType;
        u8 unk;
    } PACKED;

    static_assert(sizeof(ContentRecord) == 0x18, "ContentRecord must be 0x18!");

    struct HashedContentRecord
    {
        u8 hash[0x20];
        ContentRecord record;
    } PACKED;

    struct InstallContentMetaHeader
    {
        u16 extendedHeaderSize;
        u16 contentCount;
        u16 contentMetaCount;
        u16 padding;
    } PACKED;

    class ContentMeta final
    {
        private:
            std::vector<u8> m_contentMetaBytes;

            ContentMetaHeader m_contentMetaHeader;

        public:
            std::vector<ContentRecord> m_contentRecords;

            ContentMeta();

            Result ParseData(u8* data, size_t dataSize);
            Result GetInstallContentMeta(NcmMetaRecord *contentMetaKeyOut, NcmContentRecord& cnmtContentRecord, std::vector<u8>& installContentMetaBytesOut, bool ignoreReqFirmVersion);
    };
}