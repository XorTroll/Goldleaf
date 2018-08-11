#include "install/content_meta.hpp"

#include <cstring>
#include "debug.h"
#include "error.hpp"

// TODO: Fix printfs in here to use proper logging

namespace tin::install
{
    ContentMeta::ContentMeta() {}

    Result ContentMeta::ParseData(u8* data, size_t dataSize)
    {
        m_contentMetaBytes.resize(dataSize, 0);
        memcpy(m_contentMetaBytes.data(), data, dataSize);

        if (dataSize < sizeof(ContentMetaHeader))
        {
            LOG_DEBUG("Data size is too small! 0x%lx", dataSize);
            return -1;
        }

        memcpy(&m_contentMetaHeader, m_contentMetaBytes.data(), sizeof(ContentMetaHeader));

        for (unsigned int i = 0; i < m_contentMetaHeader.contentCount; i++)
        {
            HashedContentRecord hashedContentRecord = reinterpret_cast<HashedContentRecord*>(m_contentMetaBytes.data() + sizeof(ContentMetaHeader) + m_contentMetaHeader.extendedHeaderSize)[i];
            
            // Don't install delta fragments. Even patches don't seem to install them.
            if (hashedContentRecord.record.contentType <= 5)
                m_contentRecords.push_back(hashedContentRecord.record);
        }

        return 0;
    }

    Result ContentMeta::GetInstallContentMeta(NcmMetaRecord *contentMetaKeyOut, NcmContentRecord& cnmtContentRecord, std::vector<u8>& installContentMetaBytesOut, bool ignoreReqFirmVersion)
    {
        memset(contentMetaKeyOut, 0, sizeof(NcmMetaRecord));
        contentMetaKeyOut->titleId = m_contentMetaHeader.titleId;
        contentMetaKeyOut->version = m_contentMetaHeader.titleVersion;
        contentMetaKeyOut->type = static_cast<u8>(m_contentMetaHeader.type);

        // Setup the content meta header
        InstallContentMetaHeader installContentMetaHeader;
        installContentMetaHeader.extendedHeaderSize = m_contentMetaHeader.extendedHeaderSize;
        installContentMetaHeader.contentCount = m_contentRecords.size() + 1; // Add one for the cnmt content record
        installContentMetaHeader.contentMetaCount = m_contentMetaHeader.contentMetaCount;

        auto installContentMetaHeaderBytes = reinterpret_cast<u8*>(&installContentMetaHeader);
        installContentMetaBytesOut.insert(installContentMetaBytesOut.end(), installContentMetaHeaderBytes, installContentMetaHeaderBytes + sizeof(InstallContentMetaHeader));

        // Setup the meta extended header
        auto extendedHeaderBytes = m_contentMetaBytes.data() + sizeof(ContentMetaHeader);

        // Optionally disable the required system version field
        if (ignoreReqFirmVersion && (m_contentMetaHeader.type == ContentMetaType::APPLICATION || m_contentMetaHeader.type == ContentMetaType::PATCH))
        {
            *(u64 *)(extendedHeaderBytes + 8) = 0;
        }

        installContentMetaBytesOut.insert(installContentMetaBytesOut.end(), extendedHeaderBytes, extendedHeaderBytes + m_contentMetaHeader.extendedHeaderSize);

        // Setup cnmt content record
        auto cnmtContentRecordBytes = reinterpret_cast<u8*>(&cnmtContentRecord);
        installContentMetaBytesOut.insert(installContentMetaBytesOut.end(), cnmtContentRecordBytes, cnmtContentRecordBytes + sizeof(ContentRecord));

        // Setup the content records
        for (unsigned int i = 0; i < m_contentRecords.size(); i++)
        {
            auto contentRecordBytes = reinterpret_cast<u8*>(&m_contentRecords[i]);
            installContentMetaBytesOut.insert(installContentMetaBytesOut.end(), contentRecordBytes, contentRecordBytes + sizeof(ContentRecord));
        }

        if (m_contentMetaHeader.type == ContentMetaType::PATCH)
        {
            PatchMetaExtendedHeader* patchMetaExtendedHeader = (PatchMetaExtendedHeader*)extendedHeaderBytes;
            installContentMetaBytesOut.resize(installContentMetaBytesOut.size() + patchMetaExtendedHeader->extendedDataSize, 0);
        }

        return 0;
    }
}

