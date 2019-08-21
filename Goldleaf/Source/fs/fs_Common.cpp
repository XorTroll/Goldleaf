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
        struct stat st;
        return (stat(Path.AsUTF8().c_str(), &st) == 0);
    }

    bool IsFile(pu::String Path)
    {
        bool is = false;
        struct stat st;
        if(stat(Path.AsUTF8().c_str(), &st) == 0) if(st.st_mode & S_IFREG) is = true;
        return is;
    }

    bool IsDirectory(pu::String Path)
    {
        bool is = false;
        struct stat st;
        if(stat(Path.AsUTF8().c_str(), &st) == 0) if(st.st_mode & S_IFDIR) is = true;
        return is;
    }

    void CreateFile(pu::String Path)
    {
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, 0);
    }

    void CreateConcatenationFile(pu::String Path)
    {
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, FS_CREATE_BIG_FILE);
    }

    Result CreateDirectory(pu::String Path)
    {
        int res = mkdir(Path.AsUTF8().c_str(), 777);
        Result rc = 0;
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
    }

    void CopyFile(pu::String Path, pu::String NewPath)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Path));
        Explorer *ogexp = GetExplorerForMountName(GetPathRoot(NewPath));
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFile(Path, NewPath);
    }

    void CopyFileProgress(pu::String Path, pu::String NewPath, std::function<void(u8 Percentage)> Callback)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Path));
        Explorer *ogexp = GetExplorerForMountName(GetPathRoot(NewPath));
        auto fsize = gexp->GetFileSize(Path);
        if((fsize >= Size4GB) && (ogexp == GetSdCardExplorer())) CreateConcatenationFile(NewPath);
        gexp->CopyFileProgress(Path, NewPath, Callback);
    }

    void CopyDirectory(pu::String Dir, pu::String NewDir)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Dir));
        gexp->CopyDirectory(Dir, NewDir);
    }

    void CopyDirectoryProgress(pu::String Dir, pu::String NewDir, std::function<void(u8 Percentage)> Callback)
    {
        Explorer *gexp = GetExplorerForMountName(GetPathRoot(Dir));
        gexp->CopyDirectoryProgress(Dir, NewDir, Callback);
    }

    Result DeleteFile(pu::String Path)
    {
        int res = remove(Path.AsUTF8().c_str());
        Result rc = 0;
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
    }

    Result DeleteDirectory(pu::String Path)
    {
        Result rc = 0;
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
                if(fs::IsFile(pd)) rc = DeleteFile(pd);
                else rc = DeleteDirectory(pd);
                if(rc != 0) return rc;
            }
        }
        closedir(d);
        int res = rmdir(Path.AsUTF8().c_str());
        if(res != 0) rc = err::MakeErrno(res);
        return rc;
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

    std::vector<u8> ReadFile(pu::String Path)
    {
        std::vector<u8> file;
        FILE *fle = fopen(Path.AsUTF8().c_str(), "rb");
        if(fle)
        {
            fseek(fle, 0, SEEK_END);
            auto sz = ftell(fle);
            rewind(fle);
            file.resize(sz);
            fread(file.data(), 1, sz, fle);
        }
        fclose(fle);
        return file;
    }

    std::vector<pu::String> ReadFileLines(pu::String Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<pu::String> data;
        ifstream ifs(Path.AsUTF8());
        if(ifs.good())
        {
            std::u16string tmpline;
            u32 tmpc = 0;
            u32 tmpo = 0;
            while((tmpc < LineCount) && std::getline(ifs, tmpline))
            {
                if((tmpo < LineOffset) && (LineOffset != 0))
                {
                    tmpo++;
                    continue;
                }
                std::u16string tab = u"\t";
                while(true)
                {
                    size_t spos = tmpline.find(tab);
                    if(spos == pu::String::npos) break;
                    tmpline.replace(spos, tab.length(), u"    ");
                }
                data.push_back(tmpline);
                tmpc++;
            }
        }
        ifs.close();
        return data;
    }

    std::vector<pu::String> ReadFileFormatHex(pu::String Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<pu::String> sdata;
        FILE *f = fopen(Path.AsUTF8().c_str(), "rb");
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
            pu::String tmpline;
            pu::String tmpchr;
            u32 toff = 0;
            for(u32 i = 0; i < (rrsz + 1); i++)
            {
                if(count == 16)
                {
                    std::stringstream ostrm;
                    ostrm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (off + toff);
                    pu::String def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
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
                    pu::String def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
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

    Result WriteFile(pu::String Path, std::vector<u8> Data)
    {
        Result rc = DeleteFile(Path);
        if(rc != 0) return rc;
        FILE *fle = fopen(Path.AsUTF8().c_str(), "wb");
        if(fle)
        {
            fwrite(Data.data(), 1, Data.size(), fle);
            fflush(fle);
        }
        else return 0xcafe;
        fclose(fle);
        return rc;
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