#include "install/install.hpp"

#include <string>

#include "install/nsp.hpp"
#include "install/nsp_extracted.hpp"
#include "nx/ncm.hpp"
#include "error.hpp"

// TODO: Check which ones are still needed
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "install/install_source.h"
#include "install/nsp_xml.h"
#include "nx/ipc/es.h"
#include "nx/ipc/ncm_ext.h"
#include "nx/ipc/ns_ext.h"
#include "lib/xml.h"
#include "tinfs.h"

#ifdef __cplusplus
}
#endif

namespace tin::install
{
    std::function<bool (std::string)> IS_CNMT_FUNC = [](std::string name)
    {
        return name.substr(name.find(".") + 1) == "cnmt.xml";
    };

    std::function<bool (std::string)>  IS_TIK_FUNC = [](std::string name)
    {
        return name.substr(name.find(".") + 1) == "tik";
    };

    std::function<bool (std::string)>  IS_CERT_FUNC = [](std::string name)
    {
        return name.substr(name.find(".") + 1) == "cert";
    };

    InstallTask::InstallTask(std::unique_ptr<IGameContainer>& gameContainer, FsStorageId destStorageId) :
        m_gameContainer(std::move(gameContainer)), m_destStorageId(destStorageId)
    {

    }

    // Validate and obtain all data needed for install
    Result InstallTask::PrepareForInstall()
    {
        // Check NCA files are present
        // Check tik/cert is present
        // Create records

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
    Result InstallTask::Install()
    {
        printf("Reading cnmt.xml...\n");
        auto cnmtXMLName = m_gameContainer->FindFile(IS_CNMT_FUNC);
        size_t xmlSize;
        
        PROPAGATE_RESULT_STDOUT(m_gameContainer->GetFileSize(cnmtXMLName, &xmlSize), "Failed to get cnmt xml size");
        auto xmlBuf = std::make_unique<u8[]>(xmlSize);
        PROPAGATE_RESULT_STDOUT(m_gameContainer->ReadFile(cnmtXMLName, xmlBuf.get(), xmlSize, 0x0), "Failed to read cnmt xml");

        NcmMetaRecord metaRecord;
        NcmContentRecord *contentRecords;
        size_t numContentRecords;

        printf("Skipping xml version header\n");
        size_t xmlBufOffset = 0;

        while (strncmp((const char *)xmlBuf.get() + xmlBufOffset, "<ContentMeta>", strlen("<ContentMeta>")) != 0 && xmlBufOffset < xmlSize)
        {
            xmlBufOffset++;
        }

        if (xmlBufOffset == xmlSize)
        {
            printf("installTitle: Invalid XML file.");
            return -1;
        }

        printf("Parsing cnmt.xml...\n");
        PROPAGATE_RESULT_STDOUT(parseXML(&metaRecord, &contentRecords, &numContentRecords, (u8 *)(xmlBuf.get() + xmlBufOffset), xmlSize - xmlBufOffset), "Failed to parse XML");

        ContentStorageRecord storageRecord;
        storageRecord.metaRecord = metaRecord;
        storageRecord.storageId = FsStorageId_SdCard;

        printf("Pushing application record...\n");
        PROPAGATE_RESULT_STDOUT(nsPushApplicationRecord(metaRecord.titleId, 0x3, &storageRecord, sizeof(ContentStorageRecord)), "Failed to push application record");
        printf("Creating and registering NCA placeholders...\n");

        for (size_t i = 1; i < numContentRecords; i++)
        {
            NcmContentRecord contentRecord = contentRecords[i];
            PROPAGATE_RESULT_STDOUT(this->InstallNCA(contentRecord.ncaId), "Failed to install NCA");
        }

        printf("Writing content records...\n");
        PROPAGATE_RESULT_STDOUT(this->WriteRecords(&metaRecord, contentRecords, numContentRecords), "Failed to write content records");
        printf("Installing ticket and cert...\n");
        PROPAGATE_RESULT_STDOUT(this->InstallTicketCert(), "Failed to install ticket and cert");
        printf("Done!\n");
        free(contentRecords);
        return 0;
    }

    Result InstallTask::InstallNCA(const NcmNcaId &ncaId)
    {
        Result rc = 0;

        // TODO: It appears freezing occurs if this isn't done using snprintf?
        char ncaIdStr[FS_MAX_PATH] = {0};
        u64 ncaIdLower = __bswap64(*(u64 *)ncaId.c);
        u64 ncaIdUpper = __bswap64(*(u64 *)(ncaId.c + 0x8));
        snprintf(ncaIdStr, FS_MAX_PATH, "%lx%lx", ncaIdLower, ncaIdUpper);
        std::string ncaName = ncaIdStr;

        if (m_gameContainer->HasFile(ncaName + ".nca"))
            ncaName += ".nca";
        else if (m_gameContainer->HasFile(ncaName + ".cnmt.nca"))
            ncaName += ".cnmt.nca";
        else
        {
            fprintf(nxlinkout, "Failed to find NCA file!\n");
            return -1;
        }

        nx::ncm::ContentStorage contentStorage;
        PROPAGATE_RESULT(contentStorage.Open(m_destStorageId), "Failed to open content storage");

        size_t fileSize;
        PROPAGATE_RESULT(m_gameContainer->GetFileSize(ncaName, &fileSize), "Failed to get file size");
        u64 fileOff = 0;
        size_t readSize = 0x400000; // 4MB buff
        auto readBuffer = std::make_unique<u8[]>(readSize);

        PROPAGATE_RESULT(contentStorage.CreatePlaceholder(ncaId, ncaId, fileSize), "Failed to create a placeholder file");

        if (readBuffer == NULL) 
        {
            printf("Failed to allocate read buffer!\n");
            return -1;
        }
                
        float progress;
                
        while (fileOff < fileSize) 
        {   
            // Clear the buffer before we read anything, just to be sure    
            progress = (float)fileOff / (float)fileSize;

            if (fileOff % (0x400000 * 3) == 0)
                printf("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (fileSize / 1000000), (int)(progress * 100.0), "%");

            if (fileOff + readSize >= fileSize) readSize = fileSize - fileOff;

            PROPAGATE_RESULT(m_gameContainer->ReadFile(ncaName, readBuffer.get(), readSize, fileOff), "Failed to read file into buffer!");
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

        contentStorage.DeletePlaceholder(ncaId);
        return rc;
    }

    // TODO: Implement RAII on NcmContentMetaDatabase
    Result InstallTask::WriteRecords(const NcmMetaRecord *metaRecord, NcmContentRecord* records, size_t numRecords)
    {
        Result rc;
        NcmContentMetaDatabase contentMetaDatabase;

        if (R_FAILED(rc = ncmOpenContentMetaDatabase(m_destStorageId, &contentMetaDatabase)))
        {
            printf("Failed to open content meta database. Error code: 0x%08x\n", rc);
            return rc;
        }

        if (R_FAILED(rc = ncmContentMetaDatabaseSet(&contentMetaDatabase, metaRecord, sizeof(NcmContentRecord) * numRecords, records)))
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

    Result InstallTask::InstallTicketCert()
    {
        Result rc = 0;

        // Read the tik file and put it into a buffer
        auto tikName = m_gameContainer->FindFile(IS_TIK_FUNC);
        size_t tikSize = 0;
        PROPAGATE_RESULT(m_gameContainer->GetFileSize(tikName, &tikSize), "Failed to get tik file size");
        auto tikBuf = std::make_unique<u8[]>(tikSize);
        PROPAGATE_RESULT(m_gameContainer->ReadFile(tikName, tikBuf.get(), tikSize, 0x0), "Failed to read tik into buffer");

        // Read the cert file and put it into a buffer
        auto certName = m_gameContainer->FindFile(IS_CERT_FUNC);
        size_t certSize = 0;
        PROPAGATE_RESULT(m_gameContainer->GetFileSize(certName, &certSize), "Failed to get cert file size");
        auto certBuf = std::make_unique<u8[]>(certSize);
        PROPAGATE_RESULT(m_gameContainer->ReadFile(certName, certBuf.get(), certSize, 0x0), "Failed to read tik into buffer");

        // Finally, let's actually import the ticket
        PROPAGATE_RESULT(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");

        return rc;
    }
}

Result installTitle(InstallContext *context)
{
    if (context->sourceType == InstallSourceType_Nsp)
    {
        std::unique_ptr<tin::install::IGameContainer> container = std::make_unique<tin::install::nsp::NSPContainer>();
        PROPAGATE_RESULT(container->OpenContainer(context->path), "Failed to open NSP Container");
        tin::install::InstallTask task(container, FsStorageId_SdCard);

        PROPAGATE_RESULT(task.PrepareForInstall(), "Failed to prepare for install");
        PROPAGATE_RESULT(task.Install(), "Failed to install title");

        return 0;
    }
    else if (context->sourceType == InstallSourceType_Extracted)
    {
        std::unique_ptr<tin::install::IGameContainer> container = std::make_unique<tin::install::nsp::ExtractedNSPContainer>();
        PROPAGATE_RESULT(container->OpenContainer(context->path), "Failed to open Extracted NSP Container");
        tin::install::InstallTask task(container, FsStorageId_SdCard);

        PROPAGATE_RESULT(task.PrepareForInstall(), "Failed to prepare for install");
        PROPAGATE_RESULT(task.Install(), "Failed to install title");

        return 0;
    }

    return 0xDEAD;
}