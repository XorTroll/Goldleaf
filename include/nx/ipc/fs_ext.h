#pragma once

#include <switch.h>

typedef enum
{
    FsFileSystemType_Logo = 2,
    FsFileSystemType_ContentControl = 3,
    FsFileSystemType_ContentManual = 4,
    FsFileSystemType_ContentMeta = 5,
    FsFileSystemType_ContentData = 6,
    FsFileSystemType_ApplicationPackage = 7,
} FsFileSystemType;

Result fsOpenFileSystemWithId(const char* path, u64 fileSystemType, u64 titleId, FsFileSystem* out);