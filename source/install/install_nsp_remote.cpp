#include "install/install_nsp_remote.hpp"

#include <machine/endian.h>
#include "nx/fs.hpp"
#include "nx/ncm.hpp"
#include "util/title_util.hpp"
#include "debug.h"
#include "error.hpp"

namespace tin::install::nsp
{
    NetworkNSPInstallTask::NetworkNSPInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion, std::string url) :
        Install(destStorageId, ignoreReqFirmVersion), m_remoteNSP(url)
    {
        m_remoteNSP.RetrieveHeader();
    }

    void NetworkNSPInstallTask::ReadCNMT(nx::ncm::ContentRecord* cnmtContentRecordOut, tin::util::ByteBuffer& byteBuffer)
    {
        const PFS0FileEntry* fileEntry = m_remoteNSP.GetFileEntryByExtension("cnmt.nca");

        if (fileEntry == nullptr)
            THROW_FORMAT("Failed to find cnmt file entry!\n");

        std::string cnmtNcaName(m_remoteNSP.GetFileEntryName(fileEntry));
        NcmNcaId cnmtNcaId = tin::util::GetNcaIdFromString(cnmtNcaName);
        size_t cnmtNcaSize = fileEntry->fileSize;

        nx::ncm::ContentStorage contentStorage(m_destStorageId);

        printf("CNMT Name: %s\n", cnmtNcaName.c_str());

        // We install the cnmt nca early to read from it later
        this->InstallNCA(cnmtNcaId);
        std::string cnmtNCAFullPath = contentStorage.GetPath(cnmtNcaId);

        // Create the cnmt filesystem
        nx::fs::IFileSystem cnmtNCAFileSystem;
        ASSERT_OK(cnmtNCAFileSystem.OpenFileSystemWithId(cnmtNCAFullPath, FsFileSystemType_ContentMeta, 0), ("Failed to open content meta file system " + cnmtNCAFullPath).c_str());
        tin::install::nsp::SimpleFileSystem cnmtNCASimpleFileSystem(cnmtNCAFileSystem, "/", cnmtNCAFullPath + "/");
        
        // Find and read the cnmt file
        auto cnmtName = cnmtNCASimpleFileSystem.GetFileNameFromExtension("", "cnmt");
        auto cnmtFile = cnmtNCASimpleFileSystem.OpenFile(cnmtName);
        u64 cnmtSize = cnmtFile.GetSize();

        byteBuffer.Resize(cnmtSize);
        cnmtFile.Read(0x0, byteBuffer.GetData(), cnmtSize);

        // Prepare cnmt content record
        cnmtContentRecordOut->ncaId = cnmtNcaId;
        *(u64*)cnmtContentRecordOut->size = cnmtNcaSize & 0xFFFFFFFFFFFF;
        cnmtContentRecordOut->contentType = NcmContentType_CNMT;
    }

    void NetworkNSPInstallTask::InstallNCA(const NcmNcaId& ncaId)
    {
        const PFS0FileEntry* fileEntry = m_remoteNSP.GetFileEntryByNcaId(ncaId);
        std::string ncaFileName = m_remoteNSP.GetFileEntryName(fileEntry);
        size_t ncaSize = fileEntry->fileSize;

        printf("Installing %s to storage Id %u\n", ncaFileName.c_str(), m_destStorageId);

        nx::ncm::ContentStorage contentStorage(m_destStorageId);

        // Attempt to delete any leftover placeholders
        try
        {
            contentStorage.DeletePlaceholder(ncaId);
        }
        catch (...) {}

        LOG_DEBUG("Size: 0x%lx\n", ncaSize);
        contentStorage.CreatePlaceholder(ncaId, ncaId, ncaSize);

        auto installBlockFunc = [&] (void* blockBuf, size_t bufSize, size_t blockStartOffset, size_t ncaSize)
        {
            contentStorage.WritePlaceholder(ncaId, blockStartOffset, blockBuf, bufSize);
            float progress = (float)blockStartOffset / (float)ncaSize;
            printf("> Progress: %lu/%lu MB (%d%s)\r", (blockStartOffset / 1000000), (ncaSize / 1000000), (int)(progress * 100.0), "%");
        };

        //auto progressFunc = [&] (size_t sizeRead) {};

        m_remoteNSP.RetrieveAndProcessNCA(ncaId, installBlockFunc, nullptr);

        // Clean up the line for whatever comes next
        printf("                                                           \r");
        printf("Registering placeholder...\n");
        
        try
        {
            contentStorage.Register(ncaId, ncaId);
        }
        catch (...)
        {
            printf(("Failed to register " + ncaFileName + ". It may already exist.\n").c_str());
        }

        try
        {
            contentStorage.DeletePlaceholder(ncaId);
        }
        catch (...) {}
    }

    void NetworkNSPInstallTask::InstallTicketCert()
    {        
        // Read the tik file and put it into a buffer
        const PFS0FileEntry* tikFileEntry = m_remoteNSP.GetFileEntryByExtension("tik");
        u64 tikSize = tikFileEntry->fileSize;
        auto tikBuf = std::make_unique<u8[]>(tikSize);
        printf("> Reading tik\n");
        m_remoteNSP.m_download.BufferDataRange(tikBuf.get(), m_remoteNSP.GetDataOffset() + tikFileEntry->dataOffset, tikSize, nullptr);

        // Read the cert file and put it into a buffer
        const PFS0FileEntry* certFileEntry = m_remoteNSP.GetFileEntryByExtension("cert");
        u64 certSize = certFileEntry->fileSize;
        auto certBuf = std::make_unique<u8[]>(certSize);
        printf("> Reading cert\n");
        m_remoteNSP.m_download.BufferDataRange(certBuf.get(), m_remoteNSP.GetDataOffset() + certFileEntry->dataOffset, certSize, nullptr);

        // Finally, let's actually import the ticket
        ASSERT_OK(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");
    }
}