#include "install/install.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h>

#include "install/install_source.h"
#include "install/nsp_xml.h"
#include "ipc/es.h"
#include "ipc/ncm_ext.h"
#include "ipc/ns_ext.h"
#include "lib/xml.h"
#include "tinfs.h"

// TODO: We're not closing handles properly in the case of failures. Fix this!
// TODO: Make deletion more robust. Check if it actually exists before proceeding.
static Result installNca(InstallContext *context, const NcmNcaId *ncaId, FsStorageId destStorageId)
{
    Result rc;
    InstallFile file;
    char name[FS_MAX_PATH] = {0};
    char cnmtName[FS_MAX_PATH] = {0};
    NcmContentStorage storage;

    snprintf(name, FS_MAX_PATH-1, "%016lx%016lx.nca", __bswap64(*(u64 *)ncaId->c), __bswap64(*(u64 *)(ncaId->c + 0x8)));
    snprintf(cnmtName, FS_MAX_PATH-1, "%016lx%016lx.cnmt.nca", __bswap64(*(u64 *)ncaId->c), __bswap64(*(u64 *)(ncaId->c + 0x8)));

    if (R_FAILED(rc = openInstallFile(&file, context, name)))
    {
        if (R_FAILED(rc = openInstallFile(&file, context, cnmtName)))
        {
            printf("installNca: Failed to open file. Error code: 0x%08x\n", rc);
            return rc;
        }
        else
        {
            printf("Installing %s\n", cnmtName);
        }
    }
    else
    {
        printf("Installing %s\n", name);
    }

    if (R_FAILED(rc = ncmOpenContentStorage(destStorageId, &storage)))
    {
        printf("Failed to open content storage. Error code: 0x%08x\n", rc);
        return rc;
    }

    u64 fileOff = 0;
    u8 *readBuffer;
    size_t readSize = 0x400000; // 4MB buff

    if (R_FAILED(rc = ncmCreatePlaceHolder(&storage, ncaId, ncaId, file.size)))
    {
        printf("Failed to create a placeholder file. Error code: 0x%08x\n", rc);
        return rc;
    }

    readBuffer = malloc(0x400000);
    
    if (readBuffer == NULL) 
    {
        printf("Failed to allocate read buffer!\n");
        return -1;
    }
            
    float progress;
            
    while (fileOff < file.size) 
    {   
        // Clear the buffer before we read anything, just to be sure    
        progress = (float)fileOff / (float)file.size;

        if (fileOff % (0x400000 * 3) == 0)
            printf("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (file.size / 1000000), (int)(progress * 100.0), "%");

        if (fileOff + readSize >= file.size) readSize = file.size - fileOff;

        if (R_FAILED(rc = readInstallFile(context, &file, fileOff, readBuffer, readSize)))
        {
            printf("Failed to read file into buffer! Error code: 0x%08x\n", rc);
            free(readBuffer);
            return rc;
        }

        if (R_FAILED(rc = ncmWritePlaceHolder(&storage, ncaId, fileOff, readBuffer, readSize)))
        {
            printf("Failed to write a placeholder file. Error code: 0x%08x\n", rc);
            free(readBuffer);
            return rc;
        }

        fileOff += readSize;
    }

    // Clean up the line for whatever comes next
    printf("                                                           \r");
    free(readBuffer);

    printf("Registering placeholder...\n");

    if (R_FAILED(rc = ncmContentStorageRegister(&storage, ncaId, ncaId)) && rc != 0x805)
    {
        printf("Failed to register nca. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    ncmDeletePlaceHolder(&storage, ncaId);

    CLEANUP:
    serviceClose(&storage.s);
    closeInstallFile(context, &file);
    return rc;
}

static Result writeContentRecords(const NcmMetaRecord *metaRecord, NcmContentRecord *records, size_t numRecords, FsStorageId storageId)
{
    Result rc;
    NcmContentMetaDatabase contentMetaDatabase;

    if (R_FAILED(rc = ncmOpenContentMetaDatabase(storageId, &contentMetaDatabase)))
    {
        printf("Failed to open content meta database. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = ncmContentMetaDatabaseSet(&contentMetaDatabase, metaRecord, sizeof(NcmContentRecord) * numRecords, records)))
    {
        printf("Failed to set content records. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = ncmContentMetaDatabaseCommit(&contentMetaDatabase)))
    {
        printf("Failed to commit content records. Error code: 0x%08x\n", rc);
        return rc;
    }

    serviceClose(&contentMetaDatabase.s);

    return rc;
}

static Result installTicketCert(InstallContext *context)
{
    Result rc = 0;
    InstallFile tikFile;
    InstallFile certFile;
    void *tikBuf;
    void *certBuf;

    if (R_FAILED(rc = openInstallFileWithExt(&tikFile, context, "tik")))
    {
        printf("installTicketCert: Failed to open ticket file. Error code: 0x%08x\n", rc);
        return rc;
    }

    tikBuf = malloc(tikFile.size);

    if (R_FAILED(rc = readInstallFile(context, &tikFile, 0x0, tikBuf, tikFile.size)))
    {
        printf("installTicketCert: Failed to read tik into buffer! Error code: 0x%08x\n", rc);
        free(tikBuf);
        closeInstallFile(context, &tikFile);
        return rc;
    }

    closeInstallFile(context, &tikFile);

    if (R_FAILED(rc = openInstallFileWithExt(&certFile, context, "cert")))
    {
        printf("installTicketCert: Failed to open cert file. Error code: 0x%08x\n", rc);
        return rc;
    }

    certBuf = malloc(certFile.size);

    if (R_FAILED(rc = readInstallFile(context, &certFile, 0x0, certBuf, certFile.size)))
    {
        printf("installTicketCert: Failed to read cert into buffer! Error code: 0x%08x\n", rc);
        free(tikBuf);
        free(certBuf);
        closeInstallFile(context, &certFile);
        return rc;
    }

    closeInstallFile(context, &certFile);

    if (R_FAILED(rc = esImportTicket(tikBuf, tikFile.size, certBuf, certFile.size)))
    {
        printf("Failed to import ticket. Error code: 0x%08x\n", rc);
        return rc;
    }

    return rc;
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
Result installTitle(InstallContext *context)
{
    Result rc;
    InstallFile xmlFile;

    printf("Reading cnmt.xml...\n");
    if (R_FAILED(rc = openInstallFileWithExt(&xmlFile, context, "cnmt.xml")))
    {
        printf("installTitle: Failed to open first file with extension. Error code: 0x%08x\n", rc);
        return rc;
    }

    size_t xmlBufSize = xmlFile.size + 1;
    char *xmlBuf = calloc(1, xmlBufSize);
    size_t xmlBufOffset = 0;

    if (R_FAILED(rc = readInstallFile(context, &xmlFile, 0x0, xmlBuf, xmlFile.size)))
    {
        printf("installTitle: Failed to read xml into buffer! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }
    NcmMetaRecord metaRecord;
    NcmContentRecord *contentRecords;
    size_t numContentRecords;

    printf("Skipping xml version header\n");

    while (strncmp(xmlBuf + xmlBufOffset, "<ContentMeta>", strlen("<ContentMeta>")) != 0 && xmlBufOffset < xmlBufSize)
    {
        xmlBufOffset++;
    }

    if (xmlBufOffset == xmlBufSize)
    {
        printf("installTitle: Invalid XML file.");
        rc = -1;
        goto CLEANUP;
    }

    printf("Parsing cnmt.xml...\n");

    if (R_FAILED(rc = parseXML(&metaRecord, &contentRecords, &numContentRecords, (u8 *)(xmlBuf + xmlBufOffset), xmlBufSize - xmlBufOffset)))
    {
        printf("installTitle: Failed to parse xml! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    printf("Creating and registering NCA placeholders...\n");

    for (int i = 1; i < numContentRecords; i++)
    {
        NcmContentRecord contentRecord = contentRecords[i];

        if (R_FAILED(rc = installNca(context, &contentRecord.ncaId, FsStorageId_SdCard)))
        {
            printf("installTitle: Failed to install nca! Error code: 0x%08x\n", rc);
            goto CLEANUP;
        }
    }

    printf("Writing content records...\n");

    if (R_FAILED(rc = writeContentRecords(&metaRecord, contentRecords, numContentRecords, FsStorageId_SdCard)))
    {
        printf("installTitle: Failed to write content records! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    printf("Installing ticket and cert...\n");

    if (R_FAILED(rc = installTicketCert(context)))
    {
        printf("installTitle: Failed to write content records! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    printf("Pushing application record...\n");

    ContentStorageRecord storageRecord;
    storageRecord.metaRecord = metaRecord;
    storageRecord.storageId = FsStorageId_SdCard;

    // If there is any existing application record for this title, delete it
    if (R_FAILED(rc = nsDeleteApplicationRecord(metaRecord.titleId)) && rc != 0x410)
    {
        printf("Failed to delete application record. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = nsPushApplicationRecord(metaRecord.titleId, 0x3, &storageRecord, sizeof(ContentStorageRecord))))
    {
        printf("Failed to push application record. Error code: 0x%08x\n", rc);
        return rc;
    }

    printf("Done!\n");

    CLEANUP:
    free(contentRecords);
    free(xmlBuf);
    closeInstallFile(context, &xmlFile);
    return rc;
}