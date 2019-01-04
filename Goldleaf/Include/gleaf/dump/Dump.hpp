
#include <gleaf/fs.hpp>

namespace gleaf::dump
{
    enum class NCAType
    {
        Program,
        Meta,
        Control,
        LegalInfo,
        OfflineHtml,
        Data,
    };

    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmNcaId NCAId, std::string Path, std::function<void(u8 Percentage)> Callback);
    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmMetaRecord *out);
    FsStorageId GetApplicationLocation(u64 ApplicationId);
    u32 TerminateES();
    void RelaunchES(u32 ESFlags);
    std::string GetTitleKeyAndExportTicketData(u64 ApplicationId);
    std::string GetNCAIdPath(NcmContentStorage *st, NcmNcaId *Id);
    bool GetNCAId(NcmContentMetaDatabase *cmdb, NcmMetaRecord *rec, u64 ApplicationId, NCAType Type, NcmNcaId *out);
    bool HasTitleKeyCrypto(std::string NCAPath);
    std::string GetFormattedOutputName(NcmMetaRecord *Rec);
}