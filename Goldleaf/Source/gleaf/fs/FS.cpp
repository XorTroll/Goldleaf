#include <gleaf/fs/FS.hpp>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <memory>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

namespace gleaf::fs
{
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

    void CreateDirectory(std::string Path)
    {
        ::mkdir(Path.c_str(), 777);
    }

    void CopyFile(std::string Path, std::string NewPath)
    {
        std::ifstream ifs(Path);
        std::ofstream ofs(NewPath);
        if(ifs.good() && ofs.good()) ofs << ifs.rdbuf();
        ofs.close();
        ifs.close();
    }

    void CopyDirectory(std::string Dir, std::string NewDir)
    {
        mkdir(NewDir.c_str(), 777);
        DIR *from = opendir(Dir.c_str());
        if(from)
        {
            struct dirent *dent;
            while(true)
            {
                dent = readdir(from);
                if(dent == NULL) break;
                std::string nd = dent->d_name;
                std::string dfrom = Dir + "/" + nd;
                std::string dto = NewDir + "/" + nd;
                if(gleaf::fs::IsFile(dfrom)) CopyFile(dfrom, dto);
                else CopyDirectory(dfrom, dto);
            }
        }
        closedir(from);
    }

    void DeleteFile(std::string Path)
    {
        remove(Path.c_str());
    }

    void DeleteDirectory(std::string Path)
    {
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
                if(gleaf::fs::IsFile(pd)) DeleteFile(pd);
                else DeleteDirectory(pd);
            }
        }
        closedir(d);
        rmdir(Path.c_str());
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

    void WriteFile(std::string Path, std::vector<u8> Data)
    {
        DeleteFile(Path);
        FILE *fle = fopen(Path.c_str(), "wb");
        if(fle)
        {
            fwrite(Data.data(), 1, Data.size(), fle);
            fflush(fle);
        }
        fclose(fle);
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
                if(gleaf::fs::IsFile(pd)) sz += GetFileSize(pd);
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

    std::string GetPathWithoutRoot(std::string Path)
    {
        return Path.substr(Path.find_first_of(":") + 1);
    }

    u64 GetTotalSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        FsFileSystem fs;
        switch(Partition)
        {
            case Partition::NANDSafe:
                fsOpenBisFileSystem(&fs, 29, "");
                fsFsGetTotalSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::NANDSystem:
                fsOpenBisFileSystem(&fs, 30, "");
                fsFsGetTotalSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::NANDUser:
                fsOpenBisFileSystem(&fs, 31, "");
                fsFsGetTotalSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::SdCard:
                fsFsGetTotalSpace(fsdevGetDefaultFileSystem(), "/", &space);
                break;
        }
        return space;
    }

    u64 GetFreeSpaceForPartition(Partition Partition)
    {
        u64 space = 0;
        FsFileSystem fs;
        switch(Partition)
        {
            case Partition::NANDSafe:
                fsOpenBisFileSystem(&fs, 29, "");
                fsFsGetFreeSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::NANDSystem:
                fsOpenBisFileSystem(&fs, 30, "");
                fsFsGetFreeSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::NANDUser:
                fsOpenBisFileSystem(&fs, 31, "");
                fsFsGetFreeSpace(&fs, "/", &space);
                fsFsClose(&fs);
                break;
            case Partition::SdCard:
                fsFsGetFreeSpace(fsdevGetDefaultFileSystem(), "/", &space);
                break;
        }
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

    std::string SearchForFile(FsFileSystem FS, std::string Path, std::string Extension, std::string Root)
    {
        FsDir sddir;
        fsFsOpenDirectory(&FS, (Root + "/").c_str(), (FS_DIROPEN_FILE | FS_DIROPEN_DIRECTORY), &sddir);
        u64 count = 0;
        fsDirGetEntryCount(&sddir, &count);
        auto entries = std::make_unique<FsDirectoryEntry[]>(count);
        size_t eread;
        fsDirRead(&sddir, 0, &eread, count, entries.get());
        if(count > 0) for(u32 i = 0; i < count; i++)
        {
            FsDirectoryEntry ent = entries[i];
            std::string entname = ent.name;
            if(ent.type == ENTRYTYPE_DIR)
            {
                auto subdir = Path + entname + "/";
                auto dfound = SearchForFile(FS, subdir, Extension);
                if(dfound != "") return dfound;
                continue;
            }
            else if(ent.type == ENTRYTYPE_FILE)
            {
                auto ffound = entname.substr(entname.find(".") + 1);
                if(ffound == Extension) return entname;
            }
        }
        return "";
    }
}