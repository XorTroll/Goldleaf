#pragma once

#include "install/install_source.h"

typedef struct 
{
    u32 magic;
    u32 numFiles;
    u32 stringTableSize;
    u32 reserved;
} PFS0StartHeader;

typedef struct 
{
    u64 offset;
    u64 size;
    u32 stringTableOff;
    u32 reserved;
} PFS0FileEntry;

Result openNspInstallFile(InstallFile *out, InstallContext *context, char *name);
Result openNspInstallFileWithExt(InstallFile *out, InstallContext *context, char *ext);
Result readNspInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t bufSize);
void closeNspInstallFile(InstallContext *context, InstallFile *file);