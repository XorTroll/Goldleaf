#include "install/install_nsp.hpp"

#include <machine/endian.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
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
    Result NSPInstallTask::PrepareForInstall()
    {
        // Create the path of the cnmt NCA
        auto cnmtNCAName = m_simpleFileSystem->GetFileNameFromExtension("", "cnmt.nca");
        auto cnmtNCAFullPath = m_simpleFileSystem->m_absoluteRootPath + cnmtNCAName;

        // Create the cnmt filesystem
        nx::fs::IFileSystem cnmtNCAFileSystem;
        PROPAGATE_RESULT_STDOUT(cnmtNCAFileSystem.OpenFileSystemWithId(cnmtNCAFullPath, FsFileSystemType_ContentMeta, 0), ("Failed to open content meta file system " + cnmtNCAFullPath).c_str());
        tin::install::nsp::SimpleFileSystem cnmtNCASimpleFileSystem(cnmtNCAFileSystem, "/", cnmtNCAFullPath + "/");
        
        // Find and read the cnmt file
        auto cnmtName = cnmtNCASimpleFileSystem.GetFileNameFromExtension("", "cnmt");
        size_t cnmtSize;
        PROPAGATE_RESULT_STDOUT(cnmtNCASimpleFileSystem.GetFileSize(cnmtName, &cnmtSize), "Failed to get cnmt size");
        auto cnmtBuf = std::make_unique<u8[]>(cnmtSize);
        PROPAGATE_RESULT_STDOUT(cnmtNCASimpleFileSystem.ReadFile(cnmtName, cnmtBuf.get(), cnmtSize, 0x0), "Failed to read cnmt");

        // Parse data and create install content meta
        PROPAGATE_RESULT_STDOUT(m_contentMeta.ParseData(cnmtBuf.get(), cnmtSize), "Failed to parse data");
        
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

        PROPAGATE_RESULT_STDOUT(m_contentMeta.GetInstallContentMeta(&m_metaRecord, m_cnmtContentRecord, m_installContentMetaData), "Failed to get install content meta");

        // Check NCA files are present
        // Check tik/cert is present

        return 0;
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
    Result NSPInstallTask::Install()
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
        PROPAGATE_RESULT_STDOUT(nsPushApplicationRecord(storageRecord.metaRecord.titleId, 0x3, &storageRecord, sizeof(ContentStorageRecord)), "Failed to push application record");
        printf("Creating and registering NCA placeholders...\n");

        for (auto& record : m_contentMeta.m_contentRecords)
        {
            PROPAGATE_RESULT_STDOUT(this->InstallNCA(record.ncaId), "Failed to install NCA");
        }

        PROPAGATE_RESULT_STDOUT(this->InstallNCA(m_cnmtContentRecord.ncaId), "Failed to install cnmt NCA");

        printf("Writing content records...\n");
        PROPAGATE_RESULT_STDOUT(this->WriteRecords(), "Failed to write content records");
        printf("Installing ticket and cert...\n");
        PROPAGATE_RESULT_STDOUT(this->InstallTicketCert(), "Failed to install ticket and cert");
        printf("Done!\n");
        return 0;
    }

    Result NSPInstallTask::InstallNCA(const NcmNcaId &ncaId)
    {
        Result rc = 0;

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
            fprintf(nxlinkout, "Failed to find NCA file!\n");
            return -1;
        }

        fprintf(nxlinkout, "NcaId: %s\n", ncaName.c_str());
        fprintf(nxlinkout, "Dest storage Id: %u\n", m_destStorageId);

        nx::ncm::ContentStorage contentStorage;
        PROPAGATE_RESULT(contentStorage.Open(m_destStorageId), "Failed to open content storage");

        // Attempt to delete any leftover placeholders
        contentStorage.DeletePlaceholder(ncaId);

        size_t fileSize;
        PROPAGATE_RESULT(m_simpleFileSystem->GetFileSize(ncaName, &fileSize), "Failed to get file size");
        u64 fileOff = 0;
        size_t readSize = 0x400000; // 4MB buff
        auto readBuffer = std::make_unique<u8[]>(readSize);

        if (readBuffer == NULL) 
        {
            printf("Failed to allocate read buffer!\n");
            return -1;
        }

        fprintf(nxlinkout, "Size: 0x%lx\n", fileSize);
        PROPAGATE_RESULT(contentStorage.CreatePlaceholder(ncaId, ncaId, fileSize), "Failed to create a placeholder file");
                
        float progress;
                
        while (fileOff < fileSize) 
        {   
            // Clear the buffer before we read anything, just to be sure    
            progress = (float)fileOff / (float)fileSize;

            if (fileOff % (0x400000 * 3) == 0)
                printf("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (fileSize / 1000000), (int)(progress * 100.0), "%");

            if (fileOff + readSize >= fileSize) readSize = fileSize - fileOff;

            PROPAGATE_RESULT(m_simpleFileSystem->ReadFile(ncaName, readBuffer.get(), readSize, fileOff), "Failed to read file into buffer!");
            PROPAGATE_RESULT(contentStorage.WritePlaceholder(ncaId, fileOff, readBuffer.get(), readSize), "Failed to write a placeholder file");
            fileOff += readSize;
        }

        // Clean up the line for whatever comes next
        printf("                                                           \r");
        printf("Registering placeholder...\n");
        
        if (R_FAILED(rc = contentStorage.Register(ncaId, ncaId)) && rc != 0x805)
        {
            printf("Failed to register nca. Error code: 0x%08x\n", rc);
        }
        rc = 0;

        contentStorage.DeletePlaceholder(ncaId);
        return rc;
    }

    // TODO: Implement RAII on NcmContentMetaDatabase
    Result NSPInstallTask::WriteRecords()
    {
        Result rc;
        NcmContentMetaDatabase contentMetaDatabase;

        if (R_FAILED(rc = ncmOpenContentMetaDatabase(m_destStorageId, &contentMetaDatabase)))
        {
            printf("Failed to open content meta database. Error code: 0x%08x\n", rc);
            return rc;
        }

        printBytes(nxlinkout, (u8*)&m_metaRecord, sizeof(NcmMetaRecord), true);

        if (R_FAILED(rc = ncmContentMetaDatabaseSet(&contentMetaDatabase, &m_metaRecord, m_installContentMetaData.size(), (NcmContentRecord*)m_installContentMetaData.data())))
        {
            printf("Failed to set content records. Error code: 0x%08x\n", rc);
            serviceClose(&contentMetaDatabase.s);
            return rc;
        }

        if (R_FAILED(rc = ncmContentMetaDatabaseCommit(&contentMetaDatabase)))
        {
            printf("Failed to commit content records. Error code: 0x%08x\n", rc);
            serviceClose(&contentMetaDatabase.s);
            return rc;
        }

        serviceClose(&contentMetaDatabase.s);

        return rc;
    }

    Result NSPInstallTask::InstallTicketCert()
    {
        // Read the tik file and put it into a buffer
        auto tikName = m_simpleFileSystem->GetFileNameFromExtension("", "tik");
        size_t tikSize = 0;
        PROPAGATE_RESULT(m_simpleFileSystem->GetFileSize(tikName, &tikSize), "Failed to get tik file size");
        auto tikBuf = std::make_unique<u8[]>(tikSize);
        PROPAGATE_RESULT(m_simpleFileSystem->ReadFile(tikName, tikBuf.get(), tikSize, 0x0), "Failed to read tik into buffer");

        // Read the cert file and put it into a buffer
        auto certName = m_simpleFileSystem->GetFileNameFromExtension("", "cert");
        size_t certSize = 0;
        PROPAGATE_RESULT(m_simpleFileSystem->GetFileSize(certName, &certSize), "Failed to get cert file size");
        auto certBuf = std::make_unique<u8[]>(certSize);
        PROPAGATE_RESULT(m_simpleFileSystem->ReadFile(certName, certBuf.get(), certSize, 0x0), "Failed to read tik into buffer");

        // Finally, let's actually import the ticket
        PROPAGATE_RESULT(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");
        return 0;
    }
}

Result installTitle(InstallContext *context)
{
    if (context->sourceType == InstallSourceType_Nsp)
    {
        std::string fullPath = "@Sdcard:/" + std::string(context->path);
        nx::fs::IFileSystem fileSystem;
        PROPAGATE_RESULT(fileSystem.OpenFileSystemWithId(fullPath, FsFileSystemType_ApplicationPackage, 0), "Failed to open application package file system");
        tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, "/", fullPath + "/");
        tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

        PROPAGATE_RESULT(task.PrepareForInstall(), "Failed to prepare for install");
        PROPAGATE_RESULT(task.Install(), "Failed to install title");

        return 0;
    }
    else if (context->sourceType == InstallSourceType_Extracted)
    {
        std::string fullPath = "@Sdcard:/" + std::string(context->path);
        nx::fs::IFileSystem fileSystem;
        PROPAGATE_RESULT(fileSystem.OpenSdFileSystem(), "Failed to open SD file system");
        tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, context->path, fullPath);
        tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

        PROPAGATE_RESULT(task.PrepareForInstall(), "Failed to prepare for install");
        PROPAGATE_RESULT(task.Install(), "Failed to install title");

        return 0;
    }

    return 0xDEAD;
}