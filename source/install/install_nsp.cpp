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
    NSPInstallTask::NSPInstallTask(tin::install::nsp::SimpleFileSystem& simpleFileSystem, FsStorageId destStorageId, bool ignoreReqFirmVersion) :
        IInstallTask(destStorageId), m_ignoreReqFirmVersion(ignoreReqFirmVersion), m_simpleFileSystem(&simpleFileSystem)
    {

    }

    // Validate and obtain all data needed for install
    void NSPInstallTask::PrepareForInstall()
    {
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
        auto cnmtBuf = std::make_unique<u8[]>(cnmtSize);
        cnmtFile.Read(0x0, cnmtBuf.get(), cnmtSize);

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
        *(u64*)m_cnmtContentRecord.size = cnmtNCASize & 0xFFFFFFFFFFFF;
        m_cnmtContentRecord.type = NcmContentType_CNMT;

        if (m_ignoreReqFirmVersion)
            printf("WARNING: Required system firmware version is being IGNORED!\n");

        ASSERT_OK(m_contentMeta.GetInstallContentMeta(&m_metaRecord, m_cnmtContentRecord, m_installContentMetaData, m_ignoreReqFirmVersion), "Failed to get install content meta");

        // Check NCA files are present
        // Check tik/cert is present
    }

    void NSPInstallTask::Install()
    {
        Result rc = 0;
        std::vector<ContentStorageRecord> storageRecords;
        u64 baseTitleId = 0;
        u32 contentMetaCount = 0;

        // Updates and DLC don't share the same title id as the base game, but we
        // should be able to derive it manually.
        if (m_metaRecord.type == static_cast<u8>(ContentMetaType::APPLICATION))
        {
            baseTitleId = m_metaRecord.titleId;
        }
        else if (m_metaRecord.type == static_cast<u8>(ContentMetaType::PATCH))
        {
            baseTitleId = m_metaRecord.titleId ^ 0x800;
        }
        else if (m_metaRecord.type == static_cast<u8>(ContentMetaType::ADD_ON_CONTENT))
        {
            baseTitleId = (m_metaRecord.titleId ^ 0x1000) & ~0xFFF;
        }

        // TODO: Make custom error with result code field
        // 0x410: The record doesn't already exist
        if (R_FAILED(rc = nsCountApplicationContentMeta(baseTitleId, &contentMetaCount)) && rc != 0x410)
        {
            throw std::runtime_error("Failed to count application content meta");
        }
        rc = 0;

        LOG_DEBUG("Content meta count: %u\n", contentMetaCount);

        // Obtain any existing app record content meta and append it to our vector
        if (contentMetaCount > 0)
        {
            storageRecords.resize(contentMetaCount);
            size_t contentStorageBufSize = contentMetaCount * sizeof(ContentStorageRecord);
            auto contentStorageBuf = std::make_unique<ContentStorageRecord[]>(contentMetaCount);
            u32 entriesRead;

            ASSERT_OK(nsListApplicationRecordContentMeta(0, baseTitleId, contentStorageBuf.get(), contentStorageBufSize, &entriesRead), "Failed to list application record content meta");

            if (entriesRead != contentMetaCount)
            {
                throw std::runtime_error("Mismatch between entries read and content meta count");
            }

            memcpy(storageRecords.data(), contentStorageBuf.get(), contentStorageBufSize);
        }

        // Add our new content meta
        ContentStorageRecord storageRecord;
        storageRecord.metaRecord = m_metaRecord;
        storageRecord.storageId = m_destStorageId;
        storageRecords.push_back(storageRecord);

        // Replace the existing application records with our own
        try
        {
            nsDeleteApplicationRecord(baseTitleId);
        }
        catch (...) {}

        printf("Pushing application record...\n");
        ASSERT_OK(nsPushApplicationRecord(baseTitleId, 0x3, storageRecords.data(), storageRecords.size() * sizeof(ContentStorageRecord)), "Failed to push application record");

        printf("Writing content records...\n");
        this->WriteRecords();

        printf("Installing CNNT NCA...\n");
        this->InstallNCA(m_cnmtContentRecord.ncaId);

        printf("Installing NCAs...\n");
        for (auto& record : m_contentMeta.m_contentRecords)
        {
            this->InstallNCA(record.ncaId);
        }

        printf("Installing ticket and cert...\n");
        try
        {
            this->InstallTicketCert();
        }
        catch (std::runtime_error& e)
        {
            printf("WARNING: Ticket installation failed! This may not be an issue, depending on your usecase.\nProceed with caution!\n");
        }
    }

    void NSPInstallTask::InstallNCA(const NcmNcaId &ncaId)
    {
        char ncaIdStr[FS_MAX_PATH] = {0};
        u64 ncaIdLower = __bswap64(*(u64 *)ncaId.c);
        u64 ncaIdUpper = __bswap64(*(u64 *)(ncaId.c + 0x8));
        snprintf(ncaIdStr, FS_MAX_PATH, "%016lx%016lx", ncaIdLower, ncaIdUpper);
        std::string ncaName = ncaIdStr;

        if (m_simpleFileSystem->HasFile(ncaName + ".nca"))
            ncaName += ".nca";
        else if (m_simpleFileSystem->HasFile(ncaName + ".cnmt.nca"))
            ncaName += ".cnmt.nca";
        else
        {
            throw std::runtime_error(("Failed to find NCA file " + ncaName + ".nca/.cnmt.nca").c_str());
        }

        LOG_DEBUG("NcaId: %s\n", ncaName.c_str());
        LOG_DEBUG("Dest storage Id: %u\n", m_destStorageId);

        nx::ncm::ContentStorage contentStorage(m_destStorageId);

        // Attempt to delete any leftover placeholders
        try
        {
            contentStorage.DeletePlaceholder(ncaId);
        }
        catch (...) {}

        auto ncaFile = m_simpleFileSystem->OpenFile(ncaName);
        size_t ncaSize = ncaFile.GetSize();
        u64 fileOff = 0;
        size_t readSize = 0x400000; // 4MB buff
        auto readBuffer = std::make_unique<u8[]>(readSize);

        if (readBuffer == NULL) 
            throw std::runtime_error(("Failed to allocate read buffer for " + ncaName).c_str());

        LOG_DEBUG("Size: 0x%lx\n", ncaSize);
        contentStorage.CreatePlaceholder(ncaId, ncaId, ncaSize);
                
        float progress;
                
        while (fileOff < ncaSize) 
        {   
            // Clear the buffer before we read anything, just to be sure    
            progress = (float)fileOff / (float)ncaSize;

            if (fileOff % (0x400000 * 3) == 0)
                printf("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (ncaSize / 1000000), (int)(progress * 100.0), "%");

            if (fileOff + readSize >= ncaSize) readSize = ncaSize - fileOff;

            ncaFile.Read(fileOff, readBuffer.get(), readSize);
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
            printf(("Failed to register " + ncaName + ". It may already exist.\n").c_str());
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
            ASSERT_OK(ncmContentMetaDatabaseSet(&contentMetaDatabase, &m_metaRecord, m_installContentMetaData.size(), (NcmContentMetaRecordsHeader*)m_installContentMetaData.data()), "Failed to set content records");
            ASSERT_OK(ncmContentMetaDatabaseCommit(&contentMetaDatabase), "Failed to commit content records");
        }
        catch (std::runtime_error& e)
        {
            serviceClose(&contentMetaDatabase.s);
            throw e;
        }

        LOG_DEBUG("Post Install Records: \n");
        this->DebugPrintInstallData();
    }

    void NSPInstallTask::InstallTicketCert()
    {
        // Read the tik file and put it into a buffer
        auto tikName = m_simpleFileSystem->GetFileNameFromExtension("", "tik");
        printf("> Getting tik size\n");
        auto tikFile = m_simpleFileSystem->OpenFile(tikName);
        u64 tikSize = tikFile.GetSize();
        auto tikBuf = std::make_unique<u8[]>(tikSize);
        printf("> Reading tik\n");
        tikFile.Read(0x0, tikBuf.get(), tikSize);

        // Read the cert file and put it into a buffer
        auto certName = m_simpleFileSystem->GetFileNameFromExtension("", "cert");
        printf("> Getting cert size\n");
        auto certFile = m_simpleFileSystem->OpenFile(certName);
        u64 certSize = certFile.GetSize();
        auto certBuf = std::make_unique<u8[]>(certSize);
        printf("> Reading cert\n");
        certFile.Read(0x0, certBuf.get(), certSize);

        // Finally, let's actually import the ticket
        ASSERT_OK(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");
    }

    void NSPInstallTask::DebugPrintInstallData()
    {
        #ifdef NXLINK_DEBUG

        NcmContentMetaDatabase contentMetaDatabase;
        u64 baseTitleId;
        u64 updateTitleId;
        bool hasUpdate = true;

        if (m_metaRecord.type == static_cast<u8>(ContentMetaType::APPLICATION))
        {
            baseTitleId = m_metaRecord.titleId;
            
        }
        else if (m_metaRecord.type == static_cast<u8>(ContentMetaType::PATCH))
        {
            updateTitleId = m_metaRecord.titleId;
            baseTitleId = updateTitleId ^ 0x800;
        }
        else if (m_metaRecord.type == static_cast<u8>(ContentMetaType::ADD_ON_CONTENT))
        {
            baseTitleId = (m_metaRecord.titleId ^ 0x1000) & ~0xFFF;
        }
        else
            return;

        updateTitleId = baseTitleId ^ 0x800;

        try
        {
            NcmMetaRecord latestApplicationContentMetaKey;
            NcmMetaRecord latestPatchContentMetaKey;

            ASSERT_OK(ncmOpenContentMetaDatabase(m_destStorageId, &contentMetaDatabase), "Failed to open content meta database");
            ASSERT_OK(ncmContentMetaDatabaseGetLatestContentMetaKey(&contentMetaDatabase, baseTitleId, &latestApplicationContentMetaKey), "Failed to get latest application content meta key");
            
            try
            {
                ASSERT_OK(ncmContentMetaDatabaseGetLatestContentMetaKey(&contentMetaDatabase, updateTitleId, &latestPatchContentMetaKey), "Failed to get latest patch content meta key");
            }
            catch (std::exception& e)
            {
                hasUpdate = false;
            }

            u64 appContentRecordSize;
            u64 appContentRecordSizeRead;
            ASSERT_OK(ncmContentMetaDatabaseGetSize(&contentMetaDatabase, &latestApplicationContentMetaKey, &appContentRecordSize), "Failed to get application content record size");
            
            auto appContentRecordBuf = std::make_unique<u8[]>(appContentRecordSize);
            ASSERT_OK(ncmContentMetaDatabaseGet(&contentMetaDatabase, &latestApplicationContentMetaKey, appContentRecordSize, (NcmContentMetaRecordsHeader*)appContentRecordBuf.get(), &appContentRecordSizeRead), "Failed to get app content record size");

            if (appContentRecordSize != appContentRecordSizeRead)
            {
                throw std::runtime_error("Mismatch between app content record size and content record size read");
            }

            LOG_DEBUG("Application content meta key: \n");
            printBytes(nxlinkout, (u8*)&latestApplicationContentMetaKey, sizeof(NcmMetaRecord), true);
            LOG_DEBUG("Application content meta: \n");
            printBytes(nxlinkout, appContentRecordBuf.get(), appContentRecordSize, true);

            if (hasUpdate)
            {
                u64 patchContentRecordsSize;
                u64 patchContentRecordSizeRead;
                ASSERT_OK(ncmContentMetaDatabaseGetSize(&contentMetaDatabase, &latestPatchContentMetaKey, &patchContentRecordsSize), "Failed to get patch content record size");
            
                auto patchContentRecordBuf = std::make_unique<u8[]>(patchContentRecordsSize);
                ASSERT_OK(ncmContentMetaDatabaseGet(&contentMetaDatabase, &latestPatchContentMetaKey, patchContentRecordsSize, (NcmContentMetaRecordsHeader*)patchContentRecordBuf.get(), &patchContentRecordSizeRead), "Failed to get patch content record size");
            
                if (patchContentRecordsSize != patchContentRecordSizeRead)
                {
                    throw std::runtime_error("Mismatch between app content record size and content record size read");
                }

                LOG_DEBUG("Patch content meta key: \n");
                printBytes(nxlinkout, (u8*)&latestPatchContentMetaKey, sizeof(NcmMetaRecord), true);
                LOG_DEBUG("Patch content meta: \n");
                printBytes(nxlinkout, patchContentRecordBuf.get(), patchContentRecordsSize, true);
            }
            else
            {
                LOG_DEBUG("No update records found, or an error occurred.\n");
            }

            auto appRecordBuf = std::make_unique<u8[]>(0x100);
            u32 numEntriesRead;
            ASSERT_OK(nsListApplicationRecordContentMeta(0, baseTitleId, appRecordBuf.get(), 0x100, &numEntriesRead), "Failed to list application record content meta");

            LOG_DEBUG("Application record content meta: \n");
            printBytes(nxlinkout, appRecordBuf.get(), 0x100, true);
        }
        catch (std::runtime_error& e)
        {
            serviceClose(&contentMetaDatabase.s);
            LOG_DEBUG("Failed to log install data. Error: %s", e.what());
        }

        #endif
    }
}