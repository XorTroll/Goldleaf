
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

    static constexpr u64 Size4GB = 0x100000000;

    // Only used for testing / development
    #define QUICK_LOG(fmt, ...) { \
        FILE *f = fopen("sdmc:/QuickGleaf.log", "a"); \
        if(f) \
        { \
            fprintf(f, fmt, ##__VA_ARGS__); \
            fclose(f); \
        } \
    }

    bool Exists(pu::String Path);
    bool IsFile(pu::String Path);
    bool IsDirectory(pu::String Path);
    void CreateFile(pu::String Path);
    void CreateConcatenationFile(pu::String Path);
    void CreateDirectory(pu::String Path);
    void CopyFile(pu::String Path, pu::String NewPath);
    void CopyFileProgress(pu::String Path, pu::String NewPath, std::function<void(double Done, double Total)> Callback);
    void CopyDirectory(pu::String Dir, pu::String NewDir);
    void CopyDirectoryProgress(pu::String Dir, pu::String NewDir, std::function<void(double Done, double Total)> Callback);
    void DeleteFile(pu::String Path);
    void DeleteDirectory(pu::String Path);
    void RenameFile(pu::String Old, pu::String New);
    void RenameDirectory(pu::String Old, pu::String New);
    bool IsFileBinary(pu::String Path);
    void WriteFile(pu::String Path, std::vector<u8> Data);
    u64 GetFileSize(pu::String Path);
    u64 GetDirectorySize(pu::String Path);
    pu::String GetFileName(pu::String Path);
    pu::String GetBaseDirectory(pu::String Path);
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