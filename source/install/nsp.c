#include "install/nsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_PFS0 0x30534650

static size_t calculateFullHeaderSize(PFS0StartHeader *startHeader)
{
    return sizeof(PFS0StartHeader) + startHeader->numFiles * sizeof(PFS0FileEntry) + startHeader->stringTableSize;
}

static PFS0FileEntry *getFileEntry(u8 *header, unsigned int i)
{
    PFS0StartHeader *startHeader = (PFS0StartHeader *)header;
    if (i >= startHeader->numFiles) return NULL;
    return (PFS0FileEntry *)(header + sizeof(PFS0StartHeader) + i * sizeof(PFS0FileEntry));
}

static char *getFileName(u8 *header, PFS0FileEntry *fileEntry)
{
    PFS0StartHeader *startHeader = (PFS0StartHeader *)header;
    return (char *)(header + sizeof(PFS0StartHeader) + startHeader->numFiles * sizeof(PFS0FileEntry) + fileEntry->stringTableOff);
}

// TODO: Verify magic and other stuff
// TODO: Generalise code between both open functions

Result openNspInstallFile(InstallFile *out, InstallContext *context, char *name)
{
    Result rc = 0;
    PFS0StartHeader startHeader;
    size_t actualReadSize = 0;

    if (R_FAILED(rc = fsMountSdcard(&out->fsFileSystem)))
    {
        printf("openNspInstallFile: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = fsFsOpenFile(&out->fsFileSystem, context->path, FS_OPEN_READ, &out->fsFile)))
    {
        printf("openNspInstallFile: Failed to open file. Error code: 0x%08x\n", rc);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    if (R_FAILED(rc = fsFileRead(&out->fsFile, 0x0, &startHeader, sizeof(PFS0StartHeader), &actualReadSize)))
    {
        printf("openNspInstallFile: Failed to read nsp header into buffer! Error code: 0x%08x\n", rc);
        fsFileClose(&out->fsFile);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    if (actualReadSize != sizeof(PFS0StartHeader))
    {
        printf("openNspInstallFile: Mismatch between actual size read and start header size\n");
        rc = -1;
        fsFileClose(&out->fsFile);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    actualReadSize = 0;
    size_t fullHeaderSize = calculateFullHeaderSize(&startHeader);
    u8 *fullHeader = malloc(fullHeaderSize);

    if (R_FAILED(rc = fsFileRead(&out->fsFile, 0x0, fullHeader, fullHeaderSize, &actualReadSize)))
    {
        printf("openNspInstallFile: Failed to read nsp header into buffer! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    if (actualReadSize != fullHeaderSize)
    {
        printf("openNspInstallFile: Mismatch between actual size read and start header size\n");
        rc = -1;
        goto CLEANUP;
    }

    for (int i = 0; i < startHeader.numFiles; i++)
    {
        PFS0FileEntry *fileEntry = getFileEntry(fullHeader, i);
        char *fileEntryName = getFileName(fullHeader, fileEntry);

        if (strcmp(fileEntryName, name) == 0)
        {
            strncpy(out->name, fileEntryName, FS_MAX_PATH - 1);
            out->size = fileEntry->size;
            out->nspAbsoluteOffset = fullHeaderSize + fileEntry->offset;
        }
    }

    if (out->size == 0)
    {
        printf("openNspInstallFile: Failed to find a suitable entry in the nsp\n");
        rc = -1;
        goto CLEANUP;
    }

    CLEANUP:
    free(fullHeader);
    return rc;
}

Result openNspInstallFileWithExt(InstallFile *out, InstallContext *context, char *ext)
{
    Result rc = 0;
    PFS0StartHeader startHeader;
    size_t actualReadSize = 0;

    if (R_FAILED(rc = fsMountSdcard(&out->fsFileSystem)))
    {
        printf("openExtractedInstallFileWithExt: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = fsFsOpenFile(&out->fsFileSystem, context->path, FS_OPEN_READ, &out->fsFile)))
    {
        printf("openExtractedInstallFileWithExt: Failed to open file. Error code: 0x%08x\n", rc);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    if (R_FAILED(rc = fsFileRead(&out->fsFile, 0x0, &startHeader, sizeof(PFS0StartHeader), &actualReadSize)))
    {
        printf("openExtractedInstallFileWithExt: Failed to read nsp header into buffer! Error code: 0x%08x\n", rc);
        fsFileClose(&out->fsFile);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    if (actualReadSize != sizeof(PFS0StartHeader))
    {
        printf("openExtractedInstallFileWithExt: Mismatch between actual size read and start header size\n");
        rc = -1;
        fsFileClose(&out->fsFile);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    actualReadSize = 0;
    size_t fullHeaderSize = calculateFullHeaderSize(&startHeader);
    u8 *fullHeader = malloc(fullHeaderSize);

    if (R_FAILED(rc = fsFileRead(&out->fsFile, 0x0, fullHeader, fullHeaderSize, &actualReadSize)))
    {
        printf("openExtractedInstallFileWithExt: Failed to read nsp header into buffer! Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    if (actualReadSize != fullHeaderSize)
    {
        printf("openExtractedInstallFileWithExt: Mismatch between actual size read and start header size\n");
        rc = -1;
        goto CLEANUP;
    }

    for (int i = 0; i < startHeader.numFiles; i++)
    {
        PFS0FileEntry *fileEntry = getFileEntry(fullHeader, i);
        char *fileName = getFileName(fullHeader, fileEntry);

        if (strcmp(strchr(fileName, '.') + 1, ext) == 0)
        {
            strncpy(out->name, fileName, FS_MAX_PATH - 1);
            out->size = fileEntry->size;
            out->nspAbsoluteOffset = fullHeaderSize + fileEntry->offset;
        }
    }

    if (out->size == 0)
    {
        printf("openExtractedInstallFileWithExt: Failed to find a suitable entry in the nsp\n");
        rc = -1;
        goto CLEANUP;
    }

    CLEANUP:
    free(fullHeader);
    return rc;
}

Result readNspInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t bufSize)
{
    Result rc = 0;
    size_t actualReadSize = 0;

    if (R_FAILED(rc = fsFileRead(&file->fsFile, file->nspAbsoluteOffset + off, buff, bufSize, &actualReadSize)))
    {
        printf("readNspInstallFile: Failed to read into buffer! Error code: 0x%08x\n", rc);
        return rc;
    }

    if (actualReadSize != bufSize)
    {
        printf("readNspInstallFile: Size read 0x%lx doesn't match expected size 0x%lx", actualReadSize, bufSize);
        return -1;
    }

    return rc;
}

void closeNspInstallFile(InstallContext *context, InstallFile *file)
{
    fsFileClose(&file->fsFile);
    fsFsClose(&file->fsFileSystem);
}