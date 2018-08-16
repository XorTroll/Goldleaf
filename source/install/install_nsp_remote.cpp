#include "install/install_nsp_remote.hpp"

#include <machine/endian.h>
#include "util/title_util.hpp"
#include "debug.h"
#include "error.hpp"

namespace tin::install::nsp
{
    NetworkNSPInstallTask::NetworkNSPInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion, std::string url) :
        IInstallTask(destStorageId, ignoreReqFirmVersion), m_remoteNSP(url)
    {
        m_remoteNSP.RetrieveHeader();
    }

    void NetworkNSPInstallTask::ReadCNMT()
    {
        const PFS0FileEntry* fileEntry = m_remoteNSP.GetFileEntryByExtension("cnmt.nca");

        if (fileEntry == nullptr)
            THROW_FORMAT("Failed to find cnmt file entry!\n");

        std::string cnmtName(m_remoteNSP.GetFileEntryName(fileEntry));
        NcmNcaId cnmtNcaId = tin::util::GetNcaIdFromString(cnmtName);

        // From regular NSP installation:
        /*
        // Create the path of the cnmt NCA
        auto cnmtNCAName = m_simpleFileSystem->GetFileNameFromExtension("", "cnmt.nca");
        auto cnmtNCAFile = m_simpleFileSystem->OpenFile(cnmtNCAName);
        u64 cnmtNCASize = cnmtNCAFile.GetSize();

        auto cnmtNCAFullPath = m_simpleFileSystem->m_absoluteRootPath + cnmtNCAName;

        // Create the cnmt filesystem
        nx::fs::IFileSystem cnmtNCAFileSystem;
        ASSERT_OK(cnmtNCAFileSystem.OpenFileSystemWithId(cnmtNCAFullPath, FsFileSystemType_ContentMeta, 0), ("Failed to open content meta file system " + cnmtNCAFullPath).c_str());
        tin::install::nsp::SimpleFileSystem cnmtNCASimpleFileSystem(cnmtNCAFileSystem, "/", cnmtNCAFullPath + "/");
        
        // Find and read the cnmt file
        auto cnmtName = cnmtNCASimpleFileSystem.GetFileNameFromExtension("", "cnmt");
        auto cnmtFile = cnmtNCASimpleFileSystem.OpenFile(cnmtName);
        u64 cnmtSize = cnmtFile.GetSize();

        m_cnmtByteBuf.resize(cnmtSize, 0);
        cnmtFile.Read(0x0, m_cnmtByteBuf.data(), cnmtSize);

        // Prepare cnmt ncaid
        char lowerU64[17] = {0};
        char upperU64[17] = {0};
        memcpy(lowerU64, cnmtNCAName.c_str(), 16);
        memcpy(upperU64, cnmtNCAName.c_str() + 16, 16);

        // Prepare cnmt content record
        *(u64 *)m_cnmtContentRecord.ncaId.c = __bswap64(strtoul(lowerU64, NULL, 16));
        *(u64 *)(m_cnmtContentRecord.ncaId.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));
        *(u64*)m_cnmtContentRecord.size = cnmtNCASize & 0xFFFFFFFFFFFF;
        m_cnmtContentRecord.type = NcmContentType_CNMT;
        */
    }

    void NetworkNSPInstallTask::InstallNCA(const NcmNcaId& ncaId)
    {
        printf("Install NCA  is stubbed!\n");
    }

    void NetworkNSPInstallTask::InstallTicketCert()
    {
        printf("InstallTicketCert is stubbed!\n");
    }
}