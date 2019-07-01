
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#include <fs/fs_Explorer.hpp>

namespace dump
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

    void DecryptCopyNAX0ToNCA(NcmContentStorage *ncst, NcmNcaId NCAId, std::string Path, std::function<void(double Done, double Total)> Callback);
    bool GetMetaRecord(NcmContentMetaDatabase *metadb, u64 ApplicationId, NcmMetaRecord *out);
    FsStorageId GetApplicationLocation(u64 ApplicationId);
    std::string GetTitleKeyData(u64 ApplicationId, bool ExportData);
    std::string GetNCAIdPath(NcmContentStorage *st, NcmNcaId *Id);
    bool GetNCAId(NcmContentMetaDatabase *cmdb, NcmMetaRecord *rec, u64 ApplicationId, NCAType Type, NcmNcaId *out);
    bool HasTitleKeyCrypto(std::string NCAPath);
}