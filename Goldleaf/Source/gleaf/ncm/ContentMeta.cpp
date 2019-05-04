#include <gleaf/ncm/ContentMeta.hpp>

namespace gleaf::ncm
{
    ContentMeta::ContentMeta()
    {
        buf.Resize(sizeof(ContentMetaHeader));
    }

    ContentMeta::ContentMeta(u8* Data, size_t Size) : buf(Size)
    {
        buf.Resize(Size);
        memcpy(buf.GetData(), Data, Size);
    }

    ContentMeta::~ContentMeta()
    {
    }

    ContentMetaHeader ContentMeta::GetContentMetaHeader()
    {
        return buf.Read<ContentMetaHeader>(0);
    }

    NcmMetaRecord ContentMeta::GetContentMetaKey()
    {
        NcmMetaRecord metaRecord;
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();
        memset(&metaRecord, 0, sizeof(NcmMetaRecord));
        metaRecord.titleId = contentMetaHeader.ApplicationId;
        metaRecord.version = contentMetaHeader.TitleVersion;
        metaRecord.type = static_cast<u8>(contentMetaHeader.Type);
        return metaRecord;
    }

    std::vector<ContentRecord> ContentMeta::GetContentRecords()
    {
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();
        std::vector<ContentRecord> contentRecords;
        HashedContentRecord *hashedContentRecords = (HashedContentRecord*)(buf.GetData() + sizeof(ContentMetaHeader) + contentMetaHeader.ExtendedHeaderSize);
        for(u32 i = 0; i < contentMetaHeader.ContentCount; i++)
        {
            HashedContentRecord hashedContentRecord = hashedContentRecords[i];
            if(static_cast<u8>(hashedContentRecord.Record.Type) <= 5) contentRecords.push_back(hashedContentRecord.Record); 
        }
        return contentRecords;
    }

    void ContentMeta::GetInstallContentMeta(ByteBuffer &CNMTBuffer, ContentRecord &CNMTRecord, bool IgnoreVersion)
    {
        ContentMetaHeader contentMetaHeader = this->GetContentMetaHeader();
        std::vector<ContentRecord> contentRecords = this->GetContentRecords();
        InstallContentMetaHeader installContentMetaHeader;
        installContentMetaHeader.ExtendedHeaderSize = contentMetaHeader.ExtendedHeaderSize;
        installContentMetaHeader.ContentCount = contentRecords.size() + 1;
        installContentMetaHeader.ContentMetaCount = contentMetaHeader.ContentMetaCount;
        CNMTBuffer.Append<InstallContentMetaHeader>(installContentMetaHeader);
        CNMTBuffer.Resize(CNMTBuffer.GetSize() + contentMetaHeader.ExtendedHeaderSize);
        auto *ExtendedHeaderSourceBytes = buf.GetData() + sizeof(ContentMetaHeader);
        u8 *installExtendedHeaderStart = CNMTBuffer.GetData() + sizeof(InstallContentMetaHeader);
        memcpy(installExtendedHeaderStart, ExtendedHeaderSourceBytes, contentMetaHeader.ExtendedHeaderSize);
        if(IgnoreVersion && ((contentMetaHeader.Type == ContentMetaType::Application) || (contentMetaHeader.Type == ContentMetaType::Patch))) CNMTBuffer.Write<u32>(0, sizeof(InstallContentMetaHeader) + 8);
        CNMTBuffer.Append<ContentRecord>(CNMTRecord);
        for(auto &contentRecord : contentRecords) CNMTBuffer.Append<ContentRecord>(contentRecord);
        if(contentMetaHeader.Type == ContentMetaType::Patch)
        {
            PatchMetaExtendedHeader *patchMetaExtendedHeader = (PatchMetaExtendedHeader*)ExtendedHeaderSourceBytes;
            CNMTBuffer.Resize(CNMTBuffer.GetSize() + patchMetaExtendedHeader->ExtendedDataSize);
        }
    }
}