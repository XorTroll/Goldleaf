
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <vector>
#include <switch.h>

namespace gleaf::fs
{
    enum class Partition
    {
        NANDSafe,
        NANDSystem,
        NANDUser,
        SdCard,
    };

    bool Exists(std::string Path);
    bool IsFile(std::string Path);
    bool IsDirectory(std::string Path);
    void CreateFile(std::string Path);
    void CreateDirectory(std::string Path);
    void CopyFile(std::string Path, std::string NewPath);
    void CopyDirectory(std::string Dir, std::string NewDir);
    void DeleteFile(std::string Path);
    void DeleteDirectory(std::string Path);
    std::vector<u8> ReadFile(std::string Path);
    void WriteFile(std::string Path, std::vector<u8> Data);
    u64 GetFileSize(std::string Path);
    u64 GetDirectorySize(std::string Path);
    std::string GetFileName(std::string Path);
    std::string GetExtension(std::string Path);
    std::string GetPathWithoutRoot(std::string Path);
    u64 GetTotalSpaceForPartition(Partition Partition);
    u64 GetFreeSpaceForPartition(Partition Partition);
    std::string FormatSize(u64 Bytes);
    std::string SearchForFile(FsFileSystem FS, std::string Path, std::string Extension, std::string Root = "");
}