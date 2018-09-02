#include "nx/content_meta.hpp"

#include <string.h>
#include "util/title_util.hpp"
#include "debug.h"
#include "error.hpp"

namespace nx::ncm
{
    ContentMeta::ContentMeta(u8* data, size_t size) :
        m_bytes(size)
    {
        if (size < sizeof(ContentMetaHeader))
            throw std::runtime_error("Content meta data size is too small!");

        m_bytes.Resize(size);
        memcpy(m_bytes.GetData(), data, size);
    }

    ContentMetaHeader ContentMeta::GetContentMetaHeader()
    {
        return m_bytes.Read<ContentMetaHeader>(0);
    }

    NcmMetaRecord ContentMeta::GetContentMetaKey()
    {
        NcmMetaRecord metaRecord;
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();

        memset(&metaRecord, 0, sizeof(NcmMetaRecord));
        metaRecord.titleId = contentMetaHeader.titleId;
        metaRecord.version = contentMetaHeader.titleVersion;
        metaRecord.type = static_cast<u8>(contentMetaHeader.type);

        return metaRecord;
    }

    // TODO: Cache this
    std::vector<ContentRecord> ContentMeta::GetContentRecords()
    {
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();

        std::vector<ContentRecord> contentRecords;
        HashedContentRecord* hashedContentRecords = (HashedContentRecord*)(m_bytes.GetData() + sizeof(ContentMetaHeader) + contentMetaHeader.extendedHeaderSize);

        for (unsigned int i = 0; i < contentMetaHeader.contentCount; i++)
        {
            HashedContentRecord hashedContentRecord = hashedContentRecords[i];
            
            // Don't install delta fragments. Even patches don't seem to install them.
            if (hashedContentRecord.record.contentType <= 5)
            {
                contentRecords.push_back(hashedContentRecord.record); 
            }
        }

        return contentRecords;
    }

    void ContentMeta::GetInstallContentMeta(tin::util::ByteBuffer& installContentMetaBuffer, ContentRecord& cnmtContentRecord, bool ignoreReqFirmVersion)
    {
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();
        std::vector<ContentRecord> contentRecords = this->GetContentRecords();

        // Setup the content meta header
        InstallContentMetaHeader installContentMetaHeader;
        installContentMetaHeader.extendedHeaderSize = contentMetaHeader.extendedHeaderSize;
        installContentMetaHeader.contentCount = contentRecords.size() + 1; // Add one for the cnmt content record
        installContentMetaHeader.contentMetaCount = contentMetaHeader.contentMetaCount;

        installContentMetaBuffer.Append<InstallContentMetaHeader>(installContentMetaHeader);

        // Setup the meta extended header
        auto* extendedHeaderBytes = m_bytes.GetData() + sizeof(ContentMetaHeader);

        // Optionally disable the required system version field
        if (ignoreReqFirmVersion && (contentMetaHeader.type == ContentMetaType::APPLICATION || contentMetaHeader.type == ContentMetaType::PATCH))
        {
            *(u64 *)(extendedHeaderBytes + 8) = 0;
        }
        
        auto* extendedHeaderStart = installContentMetaBuffer.GetData() + installContentMetaBuffer.GetSize();
        installContentMetaBuffer.Resize(installContentMetaBuffer.GetSize() + contentMetaHeader.extendedHeaderSize);
        memcpy(extendedHeaderStart, extendedHeaderBytes, contentMetaHeader.extendedHeaderSize);

        // Setup cnmt content record
        installContentMetaBuffer.Append<ContentRecord>(cnmtContentRecord);

        // Setup the content records
        for (auto& contentRecord : contentRecords)
        {
            installContentMetaBuffer.Append<ContentRecord>(contentRecord);
        }

        if (contentMetaHeader.type == ContentMetaType::PATCH)
        {
            PatchMetaExtendedHeader* patchMetaExtendedHeader = (PatchMetaExtendedHeader*)extendedHeaderBytes;
            installContentMetaBuffer.Resize(installContentMetaBuffer.GetSize() + patchMetaExtendedHeader->extendedDataSize);
        }
    }
}

