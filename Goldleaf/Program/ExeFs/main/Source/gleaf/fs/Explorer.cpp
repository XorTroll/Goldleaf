#include <gleaf/fs/Explorer.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace gleaf::fs
{
    static std::vector<std::string> gmounts;
    static Explorer *esdc = NULL;
    static Explorer *eprd = NULL;
    static Explorer *ensf = NULL;
    static Explorer *enus = NULL;
    static Explorer *enss = NULL;

    bool CaseCompare(std::string a, std::string b)
    {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return (a < b);
    }

    Explorer::Explorer(Partition Base)
    {
        this->customifs = false;
        if(Base == Partition::SdCard) this->mntname = "sdmc";
        else this->mntname = this->GenerateMountName();
        switch(Base)
        {
            case Partition::PRODINFOF:
                this->dspname = "Sys-ProdInfoF";
                fsOpenBisFileSystem(&this->ifs, 28, "");
                break;
            case Partition::NANDSafe:
                this->dspname = "Sys-NANDSafe";
                fsOpenBisFileSystem(&this->ifs, 29, "");
                break;
            case Partition::NANDUser:
                this->dspname = "Sys-NANDUser";
                fsOpenBisFileSystem(&this->ifs, 30, "");
                break;
            case Partition::NANDSystem:
                this->dspname = "Sys-NANDSystem";
                fsOpenBisFileSystem(&this->ifs, 31, "");
                break;
            case Partition::SdCard:
                this->dspname = "SdCard";
                this->ifs = *fsdevGetDefaultFileSystem();
                break;
        }
        this->ecwd = this->mntname + ":/";
        if(Base != Partition::SdCard) fsdevMountDevice(this->mntname.c_str(), this->ifs);
        this->part = Base;
    }

    Explorer::Explorer(FsFileSystem IFS, std::string DisplayName, bool AutoClose)
    {
        this->customifs = true;
        this->mntname = this->GenerateMountName();
        this->ifs = IFS;
        this->dspname = DisplayName;
        this->ifsaclose = AutoClose;
        this->ecwd = this->mntname + ":/";
        fsdevMountDevice(this->mntname.c_str(), IFS);
    }

    Explorer::~Explorer()
    {
        this->Close();
    }

    bool Explorer::NavigateBack()
    {
        if(this->ecwd == (this->mntname + ":/")) return false;
        std::string parent = this->ecwd.substr(0, this->ecwd.find_last_of("/\\"));
        if(parent.substr(parent.length() - 1) == ":") parent += "/";
        this->ecwd = parent;
        return true;
    }

    bool Explorer::NavigateForward(std::string Path)
    {
        std::string newdir = this->FullPathFor(Path);
        DIR *check = opendir(newdir.c_str());
        bool ok = (check != NULL);
        if(ok) this->ecwd = newdir;
        closedir(check);
        return ok;
    }

    std::vector<std::string> Explorer::GetDirectories()
    {
        std::vector<std::string> dirs;
        struct dirent *dt;
        DIR *dir = opendir(this->ecwd.c_str());
        if(dir != NULL)
        {
            while((dt = readdir(dir)) != NULL)
            {
                std::string entname = std::string(dt->d_name);
                std::string fullent = this->ecwd + "/" + entname;
                if(IsDirectory(fullent)) dirs.push_back(entname);
            }
        }
        closedir(dir);
        std::sort(dirs.begin(), dirs.end(), CaseCompare);
        return dirs;
    }

    std::vector<std::string> Explorer::GetFiles()
    {
        std::vector<std::string> files;
        struct dirent *dt;
        DIR *dir = opendir(this->ecwd.c_str());
        if(dir != NULL)
        {
            while((dt = readdir(dir)) != NULL)
            {
                std::string entname = std::string(dt->d_name);
                std::string fullent = this->ecwd + "/" + entname;
                if(IsFile(fullent)) files.push_back(entname);
            }
        }
        closedir(dir);
        std::sort(files.begin(), files.end(), CaseCompare);
        return files;
    }

    std::vector<std::string> Explorer::GetContents()
    {
        std::vector<std::string> all;
        std::vector<std::string> dirs = this->GetDirectories();
        std::vector<std::string> files = this->GetFiles();
        if(dirs.empty() && files.empty()) return all;
        all.reserve(dirs.size() + files.size());
        all.insert(all.end(), dirs.begin(), dirs.end());
        all.insert(all.end(), files.begin(), files.end());
        return all;
    }

    std::string Explorer::GetCwd()
    {
        return this->ecwd;
    }

    std::string Explorer::GetPresentableCwd()
    {
        if(this->ecwd == (this->mntname + ":/")) return this->dspname + ":/";
        u32 mntrootsize = this->mntname.length() + 2;
        std::string cwdnoroot = this->ecwd.substr(mntrootsize);
        return this->dspname + ":/" + cwdnoroot;
    }

    bool Explorer::HasPartition()
    {
        return !this->customifs;
    }

    bool Explorer::HasCustomFS()
    {
        return this->customifs;
    }

    Partition Explorer::GetPartition()
    {
        return this->part;
    }

    FsFileSystem *Explorer::GetNativeFileSystem()
    {
        return &this->ifs;
    }

    void Explorer::MovePartition(Partition NewBase)
    {
        if(this->part == NewBase) return;
        this->Close();
        this->customifs = false;
        if(NewBase == Partition::SdCard) this->mntname = "sdmc";
        else this->mntname = this->GenerateMountName();
        switch(NewBase)
        {
            case Partition::PRODINFOF:
                this->dspname = "Sys-ProdInfoF";
                fsOpenBisFileSystem(&this->ifs, 28, "");
                break;
            case Partition::NANDSafe:
                this->dspname = "Sys-NANDSafe";
                fsOpenBisFileSystem(&this->ifs, 29, "");
                break;
            case Partition::NANDUser:
                this->dspname = "Sys-NANDUser";
                fsOpenBisFileSystem(&this->ifs, 30, "");
                break;
            case Partition::NANDSystem:
                this->dspname = "Sys-NANDSystem";
                fsOpenBisFileSystem(&this->ifs, 31, "");
                break;
            case Partition::SdCard:
                this->dspname = "SdCard";
                this->ifs = *fsdevGetDefaultFileSystem();
                break;
        }
        this->ecwd = this->mntname + ":/";
        if(NewBase != Partition::SdCard) fsdevMountDevice(this->mntname.c_str(), this->ifs);
        this->part = NewBase;
    }

    std::string Explorer::FullPathFor(std::string Path)
    {
        std::string fpath = this->ecwd;
        if(this->ecwd.substr(this->ecwd.length() - 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    std::string Explorer::FullPresentablePathFor(std::string Path)
    {
        return (this->GetPresentableCwd() + "/" + Path);
    }

    u64 Explorer::GetTotalSpaceForPath(std::string Path)
    {
        u64 space = 0;
        fsFsGetTotalSpace(&this->ifs, Path.c_str(), &space);
        return space;
    }

    u64 Explorer::GetFreeSpaceForPath(std::string Path)
    {
        u64 space = 0;
        fsFsGetFreeSpace(&this->ifs, Path.c_str(), &space);
        return space;
    }

    u64 Explorer::GetTotalSpace()
    {
        u64 space = 0;
        fsFsGetTotalSpace(&this->ifs, "/", &space);
        return space;
    }

    u64 Explorer::GetFreeSpace()
    {
        u64 space = 0;
        fsFsGetFreeSpace(&this->ifs, "/", &space);
        return space;
    }

    std::string Explorer::GenerateMountName()
    {
        std::string ret;
        std::string base = "gmount-";
        u32 ridx = rand();
        base += std::to_string(ridx);
        for(u32 i = 0; i < gmounts.size(); i++)
        {
            if(gmounts[i] == base)
            {
                ret = GenerateMountName();
                break;
            }
        }
        if(ret.empty()) ret = base;
        return ret;
    }

    void Explorer::DeleteMountName(std::string GMount)
    {
        int idx = -1;
        for(u32 i = 0; i < gmounts.size(); i++)
        {
            if(gmounts[i] == GMount)
            {
                idx = i;
                break;
            }
        }
        if(idx != -1) gmounts.erase(gmounts.begin() + idx);
    }

    void Explorer::Close()
    {
        if(this->part != Partition::SdCard)
        {
            if(this->customifs)
            {
                if(this->ifsaclose) fsdevUnmountDevice(this->mntname.c_str());
            }
            else fsdevUnmountDevice(this->mntname.c_str());
            this->DeleteMountName(this->mntname);
        }
    }

    Explorer *GetSdCardExplorer()
    {
        if(esdc == NULL) esdc = new fs::Explorer(fs::Partition::SdCard);
        return esdc;
    }

    Explorer *GetPRODINFOFExplorer()
    {
        if(eprd == NULL) eprd = new fs::Explorer(fs::Partition::PRODINFOF);
        return eprd;
    }

    Explorer *GetNANDSafeExplorer()
    {
        if(ensf == NULL) ensf = new fs::Explorer(fs::Partition::NANDSafe);
        return ensf;
    }

    Explorer *GetNANDUserExplorer()
    {
        if(enus == NULL) enus = new fs::Explorer(fs::Partition::NANDUser);
        return enus;
    }

    Explorer *GetNANDSystemExplorer()
    {
        if(enss == NULL) enss = new fs::Explorer(fs::Partition::NANDSystem);
        return enss;
    }
}