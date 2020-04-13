
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

    bool Exists(String Path);
    bool IsFile(String Path);
    bool IsDirectory(String Path);
    void CreateFile(String Path);
    void CreateConcatenationFile(String Path);
    void CreateDirectory(String Path);
    void CopyFile(String Path, String NewPath);
    void CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback);
    void CopyDirectory(String Dir, String NewDir);
    void CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback);
    void DeleteFile(String Path);
    void DeleteDirectory(String Path);
    void RenameFile(String Old, String New);
    void RenameDirectory(String Old, String New);
    bool IsFileBinary(String Path);
    void WriteFile(String Path, std::vector<u8> Data);
    u64 GetFileSize(String Path);
    u64 GetDirectorySize(String Path);
    String GetFileName(String Path);
    String GetBaseDirectory(String Path);
    String GetExtension(String Path);
    String GetPathRoot(String Path);
    String GetPathWithoutRoot(String Path);
    u64 GetTotalSpaceForPartition(Partition Partition);
    u64 GetFreeSpaceForPartition(Partition Partition);
    String FormatSize(u64 Bytes);
    String SearchForFile(FsFileSystem FS, String Extension);
    String SearchForFileInPath(String Base, String Extension);

    u8 *GetFileSystemOperationsBuffer();
    size_t GetFileSystemOperationsBufferSize();
}