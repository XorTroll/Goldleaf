#include "install/nsp_extracted.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Result openExtractedInstallFile(InstallFile *out, InstallContext *context, char *name)
{
    Result rc = 0;
    char installPath[FS_MAX_PATH];

    if (R_FAILED(rc = fsMountSdcard(&out->fsFileSystem)))
    {
        //printf("openExtractedInstallFile: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return rc;
    }

    snprintf(installPath, FS_MAX_PATH, "%s%s", context->path, name);

    if (R_FAILED(rc = fsFsOpenFile(&out->fsFileSystem, installPath, FS_OPEN_READ, &out->fsFile)))
    {
        //printf("openExtractedInstallFile: Failed to open file. Error code: 0x%08x\n", rc);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    snprintf(out->name, FS_MAX_PATH, "%s", name);

    if (R_FAILED(rc = fsFileGetSize(&out->fsFile, &out->size)))
    {
        //printf("openExtractedInstallFile: Failed to get file size. Error code: 0x%08x\n", rc);
        fsFileClose(&out->fsFile);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    return rc;
}

Result openExtractedInstallFileWithExt(InstallFile *out, InstallContext *context, char *ext)
{
    Result rc = 0;
    FsDir dir;

    if (R_FAILED(rc = fsMountSdcard(&out->fsFileSystem)))
    {
        printf("openExtractedInstallFileWithExt: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (R_FAILED(rc = fsFsOpenDirectory(&out->fsFileSystem, context->path, FS_DIROPEN_FILE, &dir)))
    {
        printf("openExtractedInstallFileWithExt: Failed to open extracted dir. Error code: 0x%08x\n", rc);
        fsFsClose(&out->fsFileSystem);
        return rc;
    }

    size_t numEntriesRead;
    FsDirectoryEntry *dirEntries = calloc(256, sizeof(FsDirectoryEntry));

    if (R_FAILED(rc = fsDirRead(&dir, 0, &numEntriesRead, 256, dirEntries)))
    {
        printf("openExtractedInstallFileWithExt: Failed to read extracted dir. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    char filePath[FS_MAX_PATH] = {0};
    size_t filePathLen;

    for (int i = 0; i < numEntriesRead; i++)
    {
        FsDirectoryEntry dirEntry = dirEntries[i];

        if (dirEntry.type != ENTRYTYPE_FILE)
            continue;

        char *currExt = strchr(dirEntry.name, '.');

        if (!strncmp(currExt + 1, ext, strlen(ext)))
        {
            if (strlen(context->path) + strlen(dirEntry.name) < FS_MAX_PATH)
            {
                // We already check the path length isn't too long
                #pragma GCC diagnostic ignored "-Wformat-truncation="
                snprintf(out->name, FS_MAX_PATH, "%s", dirEntry.name);            
                snprintf(filePath, FS_MAX_PATH, "%s%s", context->path, dirEntry.name); 
                #pragma GCC diagnostic pop
                break;
            }
            else
            {
                printf("openExtractedInstallFileWithExt: File path is too long! %s%s\n", context->path, dirEntry.name);
                continue;
            }
        }
    }

    filePathLen = strlen(filePath);

    if (filePathLen == 0)
    {
        rc = -1;
        printf("openExtractedInstallFileWithExt: Failed to find file with extension. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    if (R_FAILED(rc = fsFsOpenFile(&out->fsFileSystem, filePath, FS_OPEN_READ, &out->fsFile)))
    {
        printf("openExtractedInstallFileWithExt: Failed to open file. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    if (R_FAILED(rc = fsFileGetSize(&out->fsFile, &out->size)))
    {
        printf("openExtractedInstallFileWithExt: Failed to get file size. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    CLEANUP:
    free(dirEntries);
    fsDirClose(&dir);
    fsFsClose(&out->fsFileSystem);
    return rc;
}

Result readExtractedInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t bufSize)
{
    Result rc = 0;
    size_t actualReadSize = 0;

    if (R_FAILED(rc = fsFileRead(&file->fsFile, off, buff, bufSize, &actualReadSize)))
    {
        printf("readExtractedInstallFile: Failed to read into buffer! Error code: 0x%08x\n", rc);
        return rc;
    }

    if (actualReadSize != bufSize)
    {
        printf("readExtractedInstallFile: Size read 0x%lx doesn't match expected size 0x%lx", actualReadSize, bufSize);
        return -1;
    }

    return rc;
}

void closeExtractedInstallFile(InstallContext *context, InstallFile *file)
{
    fsFileClose(&file->fsFile);
    fsFsClose(&file->fsFileSystem);
}