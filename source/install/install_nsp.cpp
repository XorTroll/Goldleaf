#include "install/install_nsp.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <string>
#include <machine/endian.h>
#include "nx/ncm.hpp"
#include "debug.h"
#include "error.hpp"

namespace tin::install::nsp
{
    NSPInstallTask::NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId) :
        IInstallTask(destStorageId), m_simpleFileSystem(&simpleFileSystem)
    {

    }

    // Validate and obtain all data needed for install
    void NSPInstallTask::PrepareForInstall()
    {
        // Create the path of the cnmt NCA
        auto cnmtNCAName = m_simpleFileSystem->GetFileNameFromExtension("", "cnmt.nca");
        auto cnmtNCAFullPath = m_simpleFileSystem->m_absoluteRootPath + cnmtNCAName;

        // Create the cnmt filesystem
        nx::fs::IFileSystem cnmtNCAFileSystem;
        ASSERT_OK(cnmtNCAFileSystem.OpenFileSystemWithId(cnmtNCAFullPath, FsFileSystemType_ContentMeta, 0), ("Failed to open content meta file system " + cnmtNCAFullPath).c_str());
        tin::install::nsp::SimpleFileSystem cnmtNCASimpleFileSystem(cnmtNCAFileSystem, "/", cnmtNCAFullPath + "/");
        
        // Find and read the cnmt file
        auto cnmtName = cnmtNCASimpleFileSystem.GetFileNameFromExtension("", "cnmt");
        size_t cnmtSize;
        ASSERT_OK(cnmtNCASimpleFileSystem.GetFileSize(cnmtName, &cnmtSize), "Failed to get cnmt size");
        auto cnmtBuf = std::make_unique<u8[]>(cnmtSize);
        ASSERT_OK(cnmtNCASimpleFileSystem.ReadFile(cnmtName, cnmtBuf.get(), cnmtSize, 0x0), "Failed to read cnmt");

        // Parse data and create install content meta
        ASSERT_OK(m_contentMeta.ParseData(cnmtBuf.get(), cnmtSize), "Failed to parse data");
        
        // Prepare cnmt ncaid
        char lowerU64[17] = {0};
        char upperU64[17] = {0};
        memcpy(lowerU64, cnmtNCAName.c_str(), 16);
        memcpy(upperU64, cnmtNCAName.c_str() + 16, 16);

        // Prepare cnmt content record
        *(u64 *)m_cnmtContentRecord.ncaId.c = __bswap64(strtoul(lowerU64, NULL, 16));
        *(u64 *)(m_cnmtContentRecord.ncaId.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));
        *(u64*)m_cnmtContentRecord.size = cnmtSize & 0xFFFFFFFFFFFF;
        m_cnmtContentRecord.type = NcmContentType_CNMT;

        ASSERT_OK(m_contentMeta.GetInstallContentMeta(&m_metaRecord, m_cnmtContentRecord, m_installContentMetaData), "Failed to get install content meta");

        // Check NCA files are present
        // Check tik/cert is present
    }

    /*
    Summary of the installation process:

    - Parse the first xml file we find within the previously selected directory.
      If we can't find one, then bail out.

    - Create a meta record based on the contents of the xml file.

    - Attempt to uninstall any existing title using the meta record

    - Create content records. The first content record should be the special "master record", which doesn't contain
      an NcaId, but instead magic, the update title id, the required system version, and the meta record type.

    - Read NCA data from the SD card directory from earlier, based on the NcaIds in the content records.

    - Write the placeholders to registered. Remove the placeholders.

    - Import the ticket and cert data using es, also from the SD card

    - Push an ApplicationRecord.

    - Profit!
    */
    void NSPInstallTask::Install()
    {
        ContentStorageRecord storageRecord;
        storageRecord.metaRecord = m_metaRecord;
        storageRecord.storageId = FsStorageId_SdCard;

        if (storageRecord.metaRecord.type == 0x81)
        {
            storageRecord.metaRecord.titleId &= ~0x800;
            storageRecord.metaRecord.type = 0x80;
        }

        printf("Pushing application record...\n");
        ASSERT_OK(nsPushApplicationRecord(storageRecord.metaRecord.titleId, 0x3, &storageRecord, sizeof(ContentStorageRecord)), "Failed to push application record");
        
        printf("Installing NCAs...\n");
        for (auto& record : m_contentMeta.m_contentRecords)
        {
            this->InstallNCA(record.ncaId);
        }

        printf("Installing CNNT NCA...\n");
        this->InstallNCA(m_cnmtContentRecord.ncaId);

        printf("Writing content records...\n");
        this->WriteRecords();
        printf("Installing ticket and cert...\n");
        this->InstallTicketCert();
        printf("Done!\n");
    }

    void NSPInstallTask::InstallNCA(const NcmNcaId &ncaId)
    {
        // TODO: It appears freezing occurs if this isn't done using snprintf?
        char ncaIdStr[FS_MAX_PATH] = {0};
        u64 ncaIdLower = __bswap64(*(u64 *)ncaId.c);
        u64 ncaIdUpper = __bswap64(*(u64 *)(ncaId.c + 0x8));
        snprintf(ncaIdStr, FS_MAX_PATH, "%lx%lx", ncaIdLower, ncaIdUpper);
        std::string ncaName = ncaIdStr;

        if (m_simpleFileSystem->HasFile(ncaName + ".nca"))
            ncaName += ".nca";
        else if (m_simpleFileSystem->HasFile(ncaName + ".cnmt.nca"))
            ncaName += ".cnmt.nca";
        else
        {
            throw std::runtime_error(("Failed to find NCA file " + ncaName + ".nca/.cnmt.nca").c_str());
        }

        fprintf(nxlinkout, "NcaId: %s\n", ncaName.c_str());
        fprintf(nxlinkout, "Dest storage Id: %u\n", m_destStorageId);

        nx::ncm::ContentStorage contentStorage(FsStorageId_SdCard);

        // Attempt to delete any leftover placeholders
        try
        {
            contentStorage.DeletePlaceholder(ncaId);
        }
        catch (...) {}

        size_t fileSize;
        ASSERT_OK(m_simpleFileSystem->GetFileSize(ncaName, &fileSize), "Failed to get file size");
        u64 fileOff = 0;
        size_t readSize = 0x400000; // 4MB buff
        auto readBuffer = std::make_unique<u8[]>(readSize);

        if (readBuffer == NULL) 
            throw std::runtime_error(("Failed to allocate read buffer for " + ncaName).c_str());

        fprintf(nxlinkout, "Size: 0x%lx\n", fileSize);
        contentStorage.CreatePlaceholder(ncaId, ncaId, fileSize);
                
        float progress;
                
        while (fileOff < fileSize) 
        {   
            // Clear the buffer before we read anything, just to be sure    
            progress = (float)fileOff / (float)fileSize;

            if (fileOff % (0x400000 * 3) == 0)
                printf("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (fileSize / 1000000), (int)(progress * 100.0), "%");

            if (fileOff + readSize >= fileSize) readSize = fileSize - fileOff;

            ASSERT_OK(m_simpleFileSystem->ReadFile(ncaName, readBuffer.get(), readSize, fileOff), "Failed to read file into buffer!");
            contentStorage.WritePlaceholder(ncaId, fileOff, readBuffer.get(), readSize);
            fileOff += readSize;
        }

        // Clean up the line for whatever comes next
        printf("                                                           \r");
        printf("Registering placeholder...\n");
        
        try
        {
            contentStorage.Register(ncaId, ncaId);
        }
        catch (...)
        {
            printf("Failed to register nca. It may already exist.");
        }

        try
        {
            contentStorage.DeletePlaceholder(ncaId);
        }
        catch (...) {}
    }

    // TODO: Implement RAII on NcmContentMetaDatabase
    void NSPInstallTask::WriteRecords()
    {
        NcmContentMetaDatabase contentMetaDatabase;

        try
        {
            ASSERT_OK(ncmOpenContentMetaDatabase(m_destStorageId, &contentMetaDatabase), "Failed to open content meta database");
            printBytes(nxlinkout, (u8*)&m_metaRecord, sizeof(NcmMetaRecord), true);
            ASSERT_OK(ncmContentMetaDatabaseSet(&contentMetaDatabase, &m_metaRecord, m_installContentMetaData.size(), (NcmContentMetaRecordsHeader*)m_installContentMetaData.data()), "Failed to set content records");
            ASSERT_OK(ncmContentMetaDatabaseCommit(&contentMetaDatabase), "Failed to commit content records");
        }
        catch (std::exception& e)
        {
            serviceClose(&contentMetaDatabase.s);
            throw e;
        }
    }

    void NSPInstallTask::InstallTicketCert()
    {
        // Read the tik file and put it into a buffer
        auto tikName = m_simpleFileSystem->GetFileNameFromExtension("", "tik");
        size_t tikSize = 0;
        ASSERT_OK(m_simpleFileSystem->GetFileSize(tikName, &tikSize), "Failed to get tik file size");
        auto tikBuf = std::make_unique<u8[]>(tikSize);
        ASSERT_OK(m_simpleFileSystem->ReadFile(tikName, tikBuf.get(), tikSize, 0x0), "Failed to read tik into buffer");

        // Read the cert file and put it into a buffer
        auto certName = m_simpleFileSystem->GetFileNameFromExtension("", "cert");
        size_t certSize = 0;
        ASSERT_OK(m_simpleFileSystem->GetFileSize(certName, &certSize), "Failed to get cert file size");
        auto certBuf = std::make_unique<u8[]>(certSize);
        ASSERT_OK(m_simpleFileSystem->ReadFile(certName, certBuf.get(), certSize, 0x0), "Failed to read tik into buffer");

        // Finally, let's actually import the ticket
        ASSERT_OK(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");
    }
}

Result installTitle(InstallContext *context)
{
    try
    {
        if (context->sourceType == InstallSourceType_Nsp)
        {
            std::string fullPath = "@Sdcard:/" + std::string(context->path);
            nx::fs::IFileSystem fileSystem;
            ASSERT_OK(fileSystem.OpenFileSystemWithId(fullPath, FsFileSystemType_ApplicationPackage, 0), "Failed to open application package file system");
            tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, "/", fullPath + "/");
            tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

            task.PrepareForInstall();
            task.Install();

            return 0;
        }
        else if (context->sourceType == InstallSourceType_Extracted)
        {
            std::string fullPath = "@Sdcard:/" + std::string(context->path);
            nx::fs::IFileSystem fileSystem;
            ASSERT_OK(fileSystem.OpenSdFileSystem(), "Failed to open SD file system");
            tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, context->path, fullPath);
            tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

            task.PrepareForInstall();
            task.Install();

            return 0;
        }
    }
    catch (std::exception& e)
    {
        fprintf(nxlinkout, "%s", e.what());
        fprintf(stdout, "%s", e.what());
    }

    return 0xDEAD;
}