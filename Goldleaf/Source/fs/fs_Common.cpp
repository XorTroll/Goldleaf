#include <fs/fs_Common.hpp>
#include <fs/fs_Explorer.hpp>
#include <err/err_Errors.hpp>
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
    static u8 *opsbuf = NULL;
    static size_t opsbufsz = 0x800000;

    bool Exists(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) return exp->Exists(Path);
        return false;
    }

    bool IsFile(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) return exp->IsFile(Path);
        return false;
    }

    bool IsDirectory(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) return exp->IsDirectory(Path);
        return false;
    }

    void CreateFile(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) exp->CreateFile(Path);
    }

    void CreateConcatenationFile(pu::String Path)
    {
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, FS_CREATE_BIG_FILE);
    }

    void CreateDirectory(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) exp->CreateDirectory(Path);
    }

    void CopyFile(pu::String Path, pu::String NewPath)
    {
        Explorer *gexp = GetExplorerForPath(Path);
        Explorer *ogexp = GetExplorerForPath(NewPath);
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFile(Path, NewPath);
    }

    void CopyFileProgress(pu::String Path, pu::String NewPath, std::function<void(double Done, double Total)> Callback)
    {
        Explorer *gexp = GetExplorerForPath(Path);
        Explorer *ogexp = GetExplorerForPath(NewPath);
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFileProgress(Path, NewPath, Callback);
    }

    void CopyDirectory(pu::String Dir, pu::String NewDir)
    {
        Explorer *gexp = GetExplorerForPath(Dir);
        gexp->CopyDirectory(Dir, NewDir);
    }

    void CopyDirectoryProgress(pu::String Dir, pu::String NewDir, std::function<void(double Done, double Total)> Callback)
    {
        Explorer *gexp = GetExplorerForPath(Dir);
        gexp->CopyDirectoryProgress(Dir, NewDir, Callback);
    }

    void DeleteFile(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) exp->DeleteFile(Path);
    }

    void DeleteDirectory(pu::String Path)
    {
        auto exp = GetExplorerForPath(Path);
        if(exp != NULL) exp->DeleteDirectory(Path);
    }

    bool IsFileBinary(pu::String Path)
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

    void WriteFile(pu::String Path, std::vector<u8> Data)
    {
        auto exp = GetExplorerForPath(Path);
        exp->DeleteFile(Path);
        exp->WriteFileBlock(Path, Data.data(), Data.size());
    }

    u64 GetFileSize(pu::String Path)
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

    u64 GetDirectorySize(pu::String Path)
    {
        u64 sz = 0;
        DIR *d = opendir(Path.AsUTF8().c_str());
        if(d)
        {
            struct dirent *dent;
            while(true)
            {
                dent = readdir(d);
                if(dent == NULL) break;
                pu::String nd = dent->d_name;
                pu::String pd = Path + "/" + nd;
                if(fs::IsFile(pd)) sz += GetFileSize(pd);
                else sz += GetDirectorySize(pd);
            }
        }
        closedir(d);
        return sz;
    }

    pu::String GetFileName(pu::String Path)
    {
        return Path.substr(Path.find_last_of("/\\") + 1);
    }

    pu::String GetBaseDirectory(pu::String Path)
    {
        return Path.substr(0, Path.find_last_of("/\\"));
    }

    pu::String GetExtension(pu::String Path)
    {
        return Path.substr(Path.find_last_of(".") + 1);
    }

    pu::String GetPathRoot(pu::String Path)
    {
        return Path.substr(0, Path.find_first_of(":"));
    }

    pu::String GetPathWithoutRoot(pu::String Path)
    {
        return Path.substr(Path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = NULL;
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
        if(fexp != NULL) space = fexp->GetTotalSpace();
        return space;
    }

    u64 GetFreeSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        fs::Explorer *fexp = NULL;
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
        if(fexp != NULL) space = fexp->GetFreeSpace();
        return space;
    }

    pu::String FormatSize(u64 Bytes)
    {
        pu::String sufs[] = { " bytes", " KB", " MB", " GB", " TB", " PB", " EB" };
        if(Bytes == 0) return "0" + sufs[0];
        u32 plc = floor((log(Bytes) / log(1024)));
        double btnum = (double)(Bytes / pow(1024, plc));
        double rbt = ((int)(btnum * 100.0) / 100.0);
        std::stringstream strm;
        strm << rbt;
        return (strm.str() + sufs[plc]);
    }

    pu::String SearchForFileInPath(pu::String Base, pu::String Extension)
    {
        pu::String path;
        DIR *dp = opendir(Base.AsUTF8().c_str());
        if(dp)
        {
            dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                pu::String pth = pu::String(dt->d_name);
                pu::String seq = pth.substr(pth.length() - Extension.length());
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
        if(opsbuf == NULL) opsbuf = new (std::align_val_t(0x1000)) u8[opsbufsz]();
        return opsbuf;
    }

    size_t GetFileSystemOperationsBufferSize()
    {
        return opsbufsz;
    }
}