
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <switch.h>

namespace gleaf::fs
{
    enum class Partition
    {
        PRODINFOF,
        NANDSafe,
        NANDUser,
        NANDSystem,
        SdCard,
    };

    bool Exists(std::string Path);
    bool IsFile(std::string Path);
    bool IsDirectory(std::string Path);
    void CreateFile(std::string Path);
    void CreateDirectory(std::string Path);
    void CopyFile(std::string Path, std::string NewPath);
    void CopyFileProgress(std::string Path, std::string NewPath, std::function<void(u8 Percentage)> Callback);
    void CopyDirectory(std::string Dir, std::string NewDir);
    void CopyDirectoryProgress(std::string Dir, std::string NewDir, std::function<void(u8 Percentage)> Callback);
    void DeleteFile(std::string Path);
    void DeleteDirectory(std::string Path);
    bool IsFileBinary(std::string Path);

    std::vector<u8> ReadFile(std::string Path);
    std::vector<std::string> ReadFileLines(std::string Path, u32 LineOffset, u32 LineCount);
    std::vector<std::string> ReadFileFormatHex(std::string Path, u32 LineOffset, u32 LineCount);
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