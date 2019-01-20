#include <gleaf/fs/Explorer.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

namespace gleaf::fs
{
    Explorer::Explorer(Partition Base)
    {
        this->customifs = false;
        switch(Base)
        {
            case Partition::PRODINFOF:
                this->ecwd = "glprodf:/";
                fsOpenBisFileSystem(&this->ifs, 28, "");
                fsdevMountDevice("glprodf", this->ifs);
                break;
            case Partition::NANDSafe:
                this->ecwd = "glsafe:/";
                fsOpenBisFileSystem(&this->ifs, 29, "");
                fsdevMountDevice("glsafe", this->ifs);
                break;
            case Partition::NANDUser:
                this->ecwd = "gluser:/";
                fsOpenBisFileSystem(&this->ifs, 30, "");
                fsdevMountDevice("gluser", this->ifs);
                break;
            case Partition::NANDSystem:
                this->ecwd = "glsystem:/";
                fsOpenBisFileSystem(&this->ifs, 31, "");
                fsdevMountDevice("glsystem", this->ifs);
                break;
            case Partition::SdCard:
                this->ecwd = "sdmc:/";
                this->ifs = *fsdevGetDefaultFileSystem();
                break;
        }
        this->part = Base;
    }

    Explorer::Explorer(FsFileSystem IFS)
    {
        this->customifs = true;
        this->ifs = IFS;
        fsdevMountDevice("glcfs", IFS);
    }

    Explorer::~Explorer()
    {
        this->Close();
    }

    bool Explorer::NavigateBack()
    {
        if((this->ecwd == "glprodf:/") || (this->ecwd == "glsafe:/") || (this->ecwd == "gluser:/") || (this->ecwd == "glsystem:/") || (this->ecwd == "sdmc:/") || (this->ecwd == "glcfs:/")) return false;
        std::string parent = this->ecwd.substr(0, this->ecwd.find_last_of("/\\"));
        DIR *check = opendir(parent.c_str());
        bool ok = (check != NULL);
        if(ok) this->ecwd = parent;
        closedir(check);
        return ok;
    }

    bool Explorer::NavigateForward(std::string Path)
    {
        std::string newdir = this->ecwd + "/" + Path;
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

    Partition Explorer::GetPartition()
    {
        return this->part;
    }

    void Explorer::MovePartition(Partition NewBase)
    {
        if(this->part == NewBase) return;
        this->Close();
        switch(NewBase)
        {
            case Partition::PRODINFOF:
                this->ecwd = "glprodf:/";
                fsOpenBisFileSystem(&this->ifs, 28, "");
                fsdevMountDevice("glprodf", this->ifs);
                break;
            case Partition::NANDSafe:
                this->ecwd = "glsafe:/";
                fsOpenBisFileSystem(&this->ifs, 29, "");
                fsdevMountDevice("glsafe", this->ifs);
                break;
            case Partition::NANDUser:
                this->ecwd = "gluser:/";
                fsOpenBisFileSystem(&this->ifs, 30, "");
                fsdevMountDevice("gluser", this->ifs);
                break;
            case Partition::NANDSystem:
                this->ecwd = "glsystem:/";
                fsOpenBisFileSystem(&this->ifs, 31, "");
                fsdevMountDevice("glsystem", this->ifs);
                break;
            case Partition::SdCard:
                this->ecwd = "sdmc:/";
                this->ifs = *fsdevGetDefaultFileSystem();
                break;
        }
        this->part = NewBase;
    }

    std::string Explorer::FullPathFor(std::string Path)
    {
        return (this->ecwd + "/" + Path);
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
        fsFsGetTotalSpace(&this->ifs, "", &space);
        return space;
    }

    u64 Explorer::GetFreeSpace()
    {
        u64 space = 0;
        fsFsGetFreeSpace(&this->ifs, "", &space);
        return space;
    }

    void Explorer::Close()
    {
        if(this->part == Partition::PRODINFOF) fsdevUnmountDevice("glprodf");
        else if(this->part == Partition::NANDSafe) fsdevUnmountDevice("glsafe");
        else if(this->part == Partition::NANDUser) fsdevUnmountDevice("gluser");
        else if(this->part == Partition::NANDSystem) fsdevUnmountDevice("glsystem");
        else if(this->customifs) fsdevUnmountDevice("glcfs");
    }
}