
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
#include <vector>
#include <fs/fs_Common.hpp>

namespace fs
{
    enum class FileMode : u32
    {
        Read = 1,
        Write,
        Append,
    };

    class Explorer
    {
        public:
            virtual ~Explorer()
            {
            }
            
            virtual bool ShouldWarnOnWriteAccess();
            void SetNames(String MountName, String DisplayName);
            bool NavigateBack();
            bool NavigateForward(String Path);
            std::vector<String> GetContents();
            String GetMountName();
            String GetCwd();
            String GetPresentableCwd();
            inline String FullPathFor(String Path);
            inline String AbsolutePathFor(String Path);
            inline String FullPresentablePathFor(String Path);
            inline String MakeFull(String Path);
            inline bool IsFullPath(String Path);
            void CopyFile(String Path, String NewPath);
            void CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback);
            void CopyDirectory(String Dir, String NewDir);
            void CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback);
            bool IsFileBinary(String Path);
            std::vector<u8> ReadFile(String Path);
            std::vector<String> ReadFileLines(String Path, u32 LineOffset, u32 LineCount);
            std::vector<String> ReadFileFormatHex(String Path, u32 LineOffset, u32 LineCount);
            u64 GetDirectorySize(String Path);
            void DeleteDirectory(String Path);

            virtual std::vector<String> GetDirectories(String Path) = 0;
            virtual std::vector<String> GetFiles(String Path) = 0;
            virtual bool Exists(String Path) = 0;
            virtual bool IsFile(String Path) = 0;
            virtual bool IsDirectory(String Path) = 0;
            virtual void CreateFile(String Path) = 0;
            virtual void CreateDirectory(String Path) = 0;
            virtual void RenameFile(String Path, String NewName) = 0;
            virtual void RenameDirectory(String Path, String NewName) = 0;
            virtual void DeleteFile(String Path) = 0;
            virtual void DeleteDirectorySingle(String Path) = 0;
            
            virtual void StartFile(String path, FileMode mode) = 0;
            virtual u64 ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out) = 0;
            virtual u64 WriteFileBlock(String Path, u8 *Data, u64 Size) = 0;
            virtual void EndFile(FileMode mode) = 0;

            virtual u64 GetFileSize(String Path) = 0;
            virtual u64 GetTotalSpace() = 0;
            virtual u64 GetFreeSpace() = 0;
            virtual void SetArchiveBit(String Path) = 0;
        protected:
            String dspname;
            String mntname;
            String ecwd;
    };

    String Explorer::FullPathFor(String Path)
    {
        String fpath = this->ecwd;
        if(this->ecwd.substr(this->ecwd.length() - 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    String Explorer::AbsolutePathFor(String Path)
    {
        String fpath = this->mntname + ":";
        if(Path.substr(0, 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    String Explorer::FullPresentablePathFor(String Path)
    {
        String pcwd = this->GetPresentableCwd();
        String fpath = pcwd;
        if(pcwd.substr(pcwd.length() - 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    String Explorer::MakeFull(String Path)
    {
        return (this->IsFullPath(Path) ? Path : this->FullPathFor(Path));
    }

    bool Explorer::IsFullPath(String Path)
    {
        return (Path.find(":/") != String::npos);
    }
}