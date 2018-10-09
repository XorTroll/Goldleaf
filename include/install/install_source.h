#pragma once

#include <switch/services/fs.h>

typedef enum
{
    InstallSourceType_None,
    InstallSourceType_Extracted,
    InstallSourceType_Nsp,
} InstallSourceType;

typedef struct
{
    InstallSourceType sourceType;
    char path[FS_MAX_PATH];
} InstallContext;

typedef struct
{
    char name[FS_MAX_PATH];
    size_t size;
    size_t nspAbsoluteOffset;
    FsFileSystem fsFileSystem;
    FsFile fsFile;
} InstallFile;

Result openInstallFile(InstallFile *out, InstallContext *context, const char *name);
Result openInstallFileWithExt(InstallFile *out, InstallContext *context, const char *ext);
Result readInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t len);
void closeInstallFile(InstallContext *context, InstallFile *file);