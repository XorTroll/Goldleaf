#include "install/install.hpp"

#include <cstring>
#include <memory>
#include "error.hpp"

namespace tin::install
{
    IInstallTask::IInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion) :
        m_destStorageId(destStorageId), m_ignoreReqFirmVersion(ignoreReqFirmVersion)
    {}

    // Validate and obtain all data needed for install
    void IInstallTask::PrepareForInstall()
    {
        this->ReadCNMT();
        this->ParseCNMT();

        // Check NCA files are present
        // Check tik/cert is present
    }

    void IInstallTask::ParseCNMT()
    {
        // Parse data and create install content meta
        ASSERT_OK(m_contentMeta.ParseData(m_cnmtByteBuf.data(), m_cnmtByteBuf.size()), "Failed to parse data");

        if (m_ignoreReqFirmVersion)
            printf("WARNING: Required system firmware version is being IGNORED!\n");

        ASSERT_OK(m_contentMeta.GetInstallContentMeta(&m_metaRecord, m_cnmtContentRecord, m_installContentMetaData, m_ignoreReqFirmVersion), "Failed to get install content meta");
    }

    void IInstallTask::Install()
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

        printf("Installing ticket and cert...\n");
        try
        {
            this->InstallTicketCert();
        }
        catch (std::runtime_error& e)
        {
            printf("WARNING: Ticket installation failed! This may not be an issue, depending on your usecase.\nProceed with caution!\n");
        }

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
    }

    // TODO: Implement RAII on NcmContentMetaDatabase
    void IInstallTask::WriteRecords()
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

    void IInstallTask::DebugPrintInstallData()
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