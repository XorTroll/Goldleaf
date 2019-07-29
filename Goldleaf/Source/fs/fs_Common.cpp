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

    bool Exists(std::string Path)
    {
        std::ifstream ifs(Path);
        bool ex = ifs.good();
        ifs.close();
        return ex;
    }

    bool IsFile(std::string Path)
    {
        bool is = false;
        struct stat st;
        if(stat(Path.c_str(), &st) == 0) if(st.st_mode & S_IFREG) is = true;
        return is;
    }

    bool IsDirectory(std::string Path)
    {
        bool is = false;
        struct stat st;
        if(stat(Path.c_str(), &st) == 0) if(st.st_mode & S_IFDIR) is = true;
        return is;
    }

    void CreateFile(std::string Path)
    {
        std::ofstream ofs(Path);
        ofs.close();
    }

    Result CreateDirectory(std::string Path)
    {
        int res = mkdir(Path.c_str(), 777);
        Result rc = 0;
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
    }

    void CopyFile(std::string Path, std::string NewPath)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Path));
        gexp->CopyFile(Path, NewPath);
    }

    void CopyFileProgress(std::string Path, std::string NewPath, std::function<void(u8 Percentage)> Callback)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Path));
        gexp->CopyFileProgress(Path, NewPath, Callback);
    }

    void CopyDirectory(std::string Dir, std::string NewDir)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Dir));
        gexp->CopyDirectory(Dir, NewDir);
    }

    void CopyDirectoryProgress(std::string Dir, std::string NewDir, std::function<void(u8 Percentage)> Callback)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Dir));
        gexp->CopyDirectoryProgress(Dir, NewDir, Callback);
    }

    Result DeleteFile(std::string Path)
    {
        int res = remove(Path.c_str());
        Result rc = 0;
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
    }

    Result DeleteDirectory(std::string Path)
    {
        Result rc = 0;
        DIR *d = opendir(Path.c_str());
        if(d)
        {
            struct dirent *dent;
            while(true)
            {
                dent = readdir(d);
                if(dent == NULL) break;
                std::string nd = dent->d_name;
                std::string pd = Path + "/" + nd;
                if(fs::IsFile(pd)) rc = DeleteFile(pd);
                else rc = DeleteDirectory(pd);
                if(rc != 0) return rc;
            }
        }
        closedir(d);
        int res = rmdir(Path.c_str());
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
    }

    bool IsFileBinary(std::string Path)
    {
        if(GetFileSize(Path) == 0) return true;
        bool bin = false;
        FILE *f = fopen(Path.c_str(), "r");
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

    std::vector<u8> ReadFile(std::string Path)
    {
        std::vector<u8> file;
        FILE *fle = fopen(Path.c_str(), "rb");
        if(fle)
        {
            fseek(fle, 0, SEEK_END);
            auto sz = ftell(fle);
            rewind(fle);
            file = std::vector<u8>(sz);
            fread(file.data(), 1, sz, fle);
        }
        fclose(fle);
        return file;
    }

    std::vector<std::string> ReadFileLines(std::string Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<std::string> data;
        std::ifstream ifs(Path);
        if(ifs.good())
        {
            std::string tmpline;
            u32 tmpc = 0;
            u32 tmpo = 0;
            while((tmpc < LineCount) && getline(ifs, tmpline))
            {
                if((tmpo < LineOffset) && (LineOffset != 0))
                {
                    tmpo++;
                    continue;
                }
                std::string tab = "\t";
                while(true)
                {
                    size_t spos = tmpline.find(tab);
                    if(spos == std::string::npos) break;
                    tmpline.replace(spos, tab.length(), "    ");
                }
                data.push_back(tmpline);
                tmpc++;
            }
        }
        ifs.close();
        return data;
    }

    std::vector<std::string> ReadFileFormatHex(std::string Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<std::string> sdata;
        FILE *f = fopen(Path.c_str(), "rb");
        if(f)
        {
            fseek(f, 0, SEEK_END);
            u64 sz = ftell(f);
            rewind(f);
            u64 off = (16 * LineOffset);
            u64 rsz = (16 * LineCount);
            if(off >= sz) return sdata;
            u64 rrsz = std::min(sz, rsz);
            if((off + rsz) > sz) rrsz = rsz - ((off + rsz) - sz);
            fseek(f, off, SEEK_SET);
            std::vector<u8> bdata(rrsz);
            fread(bdata.data(), 1, rrsz, f);
            u32 count = 0;
            std::string tmpline;
            std::string tmpchr;
            u32 toff = 0;
            for(u32 i = 0; i < (rrsz + 1); i++)
            {
                if(count == 16)
                {
                    std::stringstream ostrm;
                    ostrm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (off + toff);
                    std::string def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
                    sdata.push_back(def);
                    toff += 16;
                    count = 0;
                    tmpline = "";
                    tmpchr = "";
                }
                else if(i == rrsz)
                {
                    if((rrsz % 16) != 0)
                    {
                        u32 miss = 16 - count;
                        for(u32 i = 0; i < miss; i++)
                        {
                            tmpline += "   ";
                            tmpchr += " ";
                        }
                    }
                    std::stringstream ostrm;
                    ostrm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (off + toff);
                    std::string def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
                    sdata.push_back(def);
                    break;
                }
                u8 byte = bdata[i];
                std::stringstream strm;
                strm << std::setw(2) << std::uppercase << std::setfill('0') << std::hex << (int)byte;
                tmpline += strm.str() + " ";
                if(isprint(byte)) tmpchr += (char)byte;
                else tmpchr += ".";
                count++;
            }
            bdata.clear();
        }
        fclose(f);
        return sdata;
    }

    Result WriteFile(std::string Path, std::vector<u8> Data)
    {
        Result rc = DeleteFile(Path);
        if(rc != 0) return rc;
        FILE *fle = fopen(Path.c_str(), "wb");
        if(fle)
        {
            fwrite(Data.data(), 1, Data.size(), fle);
            fflush(fle);
        }
        else return 0xcafe;
        fclose(fle);
        return rc;
    }

    u64 GetFileSize(std::string Path)
    {
        u64 sz = 0;
        std::ifstream ifs(Path, std::ios::binary | std::ios::ate);
        if(ifs.good()) sz = ifs.tellg();
        ifs.close();
        return sz;
    }

    u64 GetDirectorySize(std::string Path)
    {
        u64 sz = 0;
        DIR *d = opendir(Path.c_str());
        if(d)
        {
            struct dirent *dent;
            while(true)
            {
                dent = readdir(d);
                if(dent == NULL) break;
                std::string nd = dent->d_name;
                std::string pd = Path + "/" + nd;
                if(fs::IsFile(pd)) sz += GetFileSize(pd);
                else sz += GetDirectorySize(pd);
            }
        }
        closedir(d);
        return sz;
    }

    std::string GetFileName(std::string Path)
    {
        return Path.substr(Path.find_last_of("/\\") + 1);
    }

    std::string GetExtension(std::string Path)
    {
        return Path.substr(Path.find_last_of(".") + 1);
    }

    std::string GetPathRoot(std::string Path)
    {
        return Path.substr(0, Path.find_first_of(":"));
    }

    std::string GetPathWithoutRoot(std::string Path)
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

    std::string FormatSize(u64 Bytes)
    {
        std::string sufs[] = { " bytes", " KB", " MB", " GB", " TB", " PB", " EB" };
        if(Bytes == 0) return "0" + sufs[0];
        u32 plc = floor((log(Bytes) / log(1024)));
        double btnum = (double)(Bytes / pow(1024, plc));
        double rbt = ((int)(btnum * 100.0) / 100.0);
        std::stringstream strm;
        strm << rbt;
        return (strm.str() + sufs[plc]);
    }

    std::string SearchForFileInPath(std::string Base, std::string Extension)
    {
        std::string path;
        DIR *dp = opendir(Base.c_str());
        if(dp)
        {
            dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                std::string pth = std::string(dt->d_name);
                std::string seq = pth.substr(pth.length() - Extension.length());
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
        if(opsbuf == NULL) opsbuf = new u8[opsbufsz];
        return opsbuf;
    }

    size_t GetFileSystemOperationsBufferSize()
    {
        return opsbufsz;
    }
}