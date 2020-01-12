
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

#include <fs/fs_FileSystem.hpp>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/stat.h>

namespace fs
{
    static u8 *opsbuf = nullptr;
    static size_t opsbufsz = 0x800000;

    bool Exists(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) return exp->Exists(Path);
        return false;
    }

    bool IsFile(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) return exp->IsFile(Path);
        return false;
    }

    bool IsDirectory(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) return exp->IsDirectory(Path);
        return false;
    }

    void CreateFile(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) exp->CreateFile(Path);
    }

    void CreateConcatenationFile(String Path)
    {
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, FsCreateOption_BigFile);
    }

    void CreateDirectory(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) exp->CreateDirectory(Path);
    }

    void CopyFile(String Path, String NewPath)
    {
        Explorer *gexp = GetExplorerForPath(Path);
        Explorer *ogexp = GetExplorerForPath(NewPath);
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFile(Path, NewPath);
    }

    void CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback)
    {
        Explorer *gexp = GetExplorerForPath(Path);
        Explorer *ogexp = GetExplorerForPath(NewPath);
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFileProgress(Path, NewPath, Callback);
    }

    void CopyDirectory(String Dir, String NewDir)
    {
        Explorer *gexp = GetExplorerForPath(Dir);
        gexp->CopyDirectory(Dir, NewDir);
    }

    void CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback)
    {
        Explorer *gexp = GetExplorerForPath(Dir);
        gexp->CopyDirectoryProgress(Dir, NewDir, Callback);
    }

    void DeleteFile(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) exp->DeleteFile(Path);
    }

    void DeleteDirectory(String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != nullptr) exp->DeleteDirectory(Path);
    }

    void RenameFile(String Old, String New)
    {
        auto exp = GetExplorerForPath(Old);
        if(exp != nullptr) exp->RenameFile(Old, New);
    }

    void RenameDirectory(String Old, String New)
    {
        auto exp = GetExplorerForPath(Old);
        if(exp != nullptr) exp->RenameDirectory(Old, New);
    }

    bool IsFileBinary(String Path)
    {
        if(GetFileSize(Path) == 0) return true;
        bool bin = false;
        FILE *f = fopen(Path.AsUTF8().c_str(), "r");
        if(f)
        {
            int ch = 0;
            while((ch = fgetc(f)) != EOF)
            {
                if(!isascii(ch) || (iscntrl(ch) && !isspace(ch)))
                {
                    bin = true;
                    break;
                }
            }
        }
        fclose(f);
        return bin;
    }

    void WriteFile(String Path, std::vector<u8> Data)
    {
        auto exp = GetExplorerForPath(Path);
        exp->DeleteFile(Path);
        exp->WriteFileBlock(Path, Data.data(), Data.size());
    }

    u64 GetFileSize(String Path)
    {
        u64 sz = 0;
        FILE *f = fopen(Path.AsUTF8().c_str(), "rb");
        if(f)
        {
            fseek(f, 0, SEEK_END);
            sz = ftell(f);
            rewind(f);
            fclose(f);
        }
        return sz;
    }

    u64 GetDirectorySize(String Path)
    {
        u64 sz = 0;
        DIR *d = opendir(Path.AsUTF8().c_str());
        if(d)
        {
            struct dirent *dent;
            while(true)
            {
                dent = readdir(d);
                if(dent == nullptr) break;
                String nd = dent->d_name;
                String pd = Path + "/" + nd;
                if(fs::IsFile(pd)) sz += GetFileSize(pd);
                else sz += GetDirectorySize(pd);
            }
        }
        closedir(d);
        return sz;
    }

    String GetFileName(String Path)
    {
        return Path.substr(Path.find_last_of("/\\") + 1);
    }

    String GetBaseDirectory(String Path)
    {
        return Path.substr(0, Path.find_last_of("/\\"));
    }

    String GetExtension(String Path)
    {
        return Path.substr(Path.find_last_of(".") + 1);
    }

    String GetPathRoot(String Path)
    {
        return Path.substr(0, Path.find_first_of(":"));
    }

    String GetPathWithoutRoot(String Path)
    {
        return Path.substr(Path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = nullptr;
        switch(Partition)
        {
            case Partition::PRODINFOF:
                fexp = fs::GetPRODINFOFExplorer();
                break;
            case Partition::NANDSafe:
                fexp = fs::GetNANDSafeExplorer();
                break;
            case Partition::NANDSystem:
                fexp = fs::GetNANDSystemExplorer();
                break;
            case Partition::NANDUser:
                fexp = fs::GetNANDUserExplorer();
                break;
            case Partition::SdCard:
                fexp = fs::GetSdCardExplorer();
                break;
        }
        if(fexp != nullptr) space = fexp->GetTotalSpace();
        return space;
    }

    u64 GetFreeSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = nullptr;
        switch(Partition)
        {
            case Partition::PRODINFOF:
                fexp = fs::GetPRODINFOFExplorer();
                break;
            case Partition::NANDSafe:
                fexp = fs::GetNANDSafeExplorer();
                break;
            case Partition::NANDSystem:
                fexp = fs::GetNANDSystemExplorer();
                break;
            case Partition::NANDUser:
                fexp = fs::GetNANDUserExplorer();
                break;
            case Partition::SdCard:
                fexp = fs::GetSdCardExplorer();
                break;
        }
        if(fexp != nullptr) space = fexp->GetFreeSpace();
        return space;
    }

    String FormatSize(u64 Bytes)
    {
        String sufs[] = { " bytes", " KB", " MB", " GB", " TB", " PB", " EB" };
        if(Bytes == 0) return "0" + sufs[0];
        u32 plc = floor((log(Bytes) / log(1024)));
        double btnum = (double)(Bytes / pow(1024, plc));
        double rbt = ((int)(btnum * 100.0) / 100.0);
        std::stringstream strm;
        strm << rbt;
        return (strm.str() + sufs[plc]);
    }

    String SearchForFileInPath(String Base, String Extension)
    {
        String path;
        DIR *dp = opendir(Base.AsUTF8().c_str());
        if(dp)
        {
            dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == nullptr) break;
                String pth = String(dt->d_name);
                String seq = pth.substr(pth.length() - Extension.length());
                if(seq == Extension)
                {
                    path = pth;
                    break;
                }
            }
        }
        closedir(dp);
        return path;
    }

    u8 *GetFileSystemOperationsBuffer()
    {
        if(opsbuf == nullptr) opsbuf = new (std::align_val_t(0x1000)) u8[opsbufsz]();
        memset(opsbuf, 0, opsbufsz);
        return opsbuf;
    }

    size_t GetFileSystemOperationsBufferSize()
    {
        return opsbufsz;
    }
}