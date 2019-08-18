
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <switch.h>
#include <Types.hpp>

namespace fs
{
    enum class Partition
    {
        PRODINFOF,
        NANDSafe,
        NANDUser = 4,
        NANDSystem = 3,
        SdCard = 5,
    };

    using ifstream = std::basic_ifstream<char16_t>;
    using ofstream = std::basic_ofstream<char16_t>;

    bool Exists(pu::String Path);
    bool IsFile(pu::String Path);
    bool IsDirectory(pu::String Path);
    void CreateFile(pu::String Path);
    Result CreateDirectory(pu::String Path);
    void CopyFile(pu::String Path, pu::String NewPath);
    void CopyFileProgress(pu::String Path, pu::String NewPath, std::function<void(u8 Percentage)> Callback);
    void CopyDirectory(pu::String Dir, pu::String NewDir);
    void CopyDirectoryProgress(pu::String Dir, pu::String NewDir, std::function<void(u8 Percentage)> Callback);
    Result DeleteFile(pu::String Path);
    Result DeleteDirectory(pu::String Path);
    bool IsFileBinary(pu::String Path);
    std::vector<u8> ReadFile(pu::String Path);
    std::vector<pu::String> ReadFileLines(pu::String Path, u32 LineOffset, u32 LineCount);
    std::vector<pu::String> ReadFileFormatHex(pu::String Path, u32 LineOffset, u32 LineCount);
    Result WriteFile(pu::String Path, std::vector<u8> Data);
    u64 GetFileSize(pu::String Path);
    u64 GetDirectorySize(pu::String Path);
    pu::String GetFileName(pu::String Path);
    pu::String GetExtension(pu::String Path);
    pu::String GetPathRoot(pu::String Path);
    pu::String GetPathWithoutRoot(pu::String Path);
    u64 GetTotalSpaceForPartition(Partition Partition);
    u64 GetFreeSpaceForPartition(Partition Partition);
    pu::String FormatSize(u64 Bytes);
    pu::String SearchForFile(FsFileSystem FS, pu::String Extension);
    pu::String SearchForFileInPath(pu::String Base, pu::String Extension);

    u8 *GetFileSystemOperationsBuffer();
    size_t GetFileSystemOperationsBufferSize();
}