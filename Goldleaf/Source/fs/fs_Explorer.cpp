#include <fs/fs_Explorer.hpp>
#include <usb/usb_Communications.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

namespace fs
{
    static Explorer *esdc = NULL;
    static Explorer *eprd = NULL;
    static Explorer *ensf = NULL;
    static Explorer *enus = NULL;
    static Explorer *enss = NULL;
    static Explorer *epcdrv = NULL;

    bool InternalCaseCompare(std::string a, std::string b)
    {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return (a < b);
    }

    Explorer::~Explorer()
    {
        this->Close();
    }

    void Explorer::Close()
    {
    }

    bool Explorer::ShouldWarnOnWriteAccess()
    {
        return false;
    }

    void Explorer::SetNames(std::string MountName, std::string DisplayName)
    {
        this->dspname = DisplayName;
        this->mntname = MountName;
        this->ecwd = MountName + ":/";
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
        bool idir = this->IsDirectory(Path);
        if(idir) this->ecwd = this->MakeFull(Path);
        return idir;
    }

    std::vector<std::string> Explorer::GetContents()
    {
        std::vector<std::string> all;
        std::vector<std::string> dirs = this->GetDirectories(this->ecwd);
        std::vector<std::string> files = this->GetFiles(this->ecwd);
        if(dirs.empty() && files.empty()) return all;
        if(!dirs.empty()) std::sort(dirs.begin(), dirs.end(), InternalCaseCompare);
        if(!files.empty()) std::sort(files.begin(), files.end(), InternalCaseCompare);
        all.reserve(dirs.size() + files.size());
        all.insert(all.end(), dirs.begin(), dirs.end());
        all.insert(all.end(), files.begin(), files.end());
        return all;
    }

    std::string Explorer::GetMountName()
    {
        return this->mntname;
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

    std::string Explorer::FullPathFor(std::string Path)
    {
        std::string fpath = this->ecwd;
        if(this->ecwd.substr(this->ecwd.length() - 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    std::string Explorer::FullPresentablePathFor(std::string Path)
    {
        std::string pcwd = this->GetPresentableCwd();
        std::string fpath = pcwd;
        if(pcwd.substr(pcwd.length() - 1) != "/") fpath += "/";
        fpath += Path;
        return fpath;
    }

    std::string Explorer::MakeFull(std::string Path)
    {
        return (this->IsFullPath(Path) ? Path : this->FullPathFor(Path));
    }

    bool Explorer::IsFullPath(std::string Path)
    {
        return (Path.find(":/") != std::string::npos);
    }

    void Explorer::CopyFile(std::string Path, std::string NewPath)
    {
        std::string path = this->MakeFull(Path);
        auto ex = GetExplorerForMountName(GetPathRoot(NewPath));
        u64 fsize = this->GetFileSize(path);
        u64 rsize = GetFileSystemOperationsBufferSize();
        u8 *data = GetFileSystemOperationsBuffer();
        u64 szrem = fsize;
        u64 off = 0;
        while(szrem)
        {
            u64 rbytes = this->ReadFileBlock(path, off, std::min(szrem, rsize), data);
            szrem -= rbytes;
            off += rbytes;
            ex->WriteFileBlock(NewPath, data, rbytes);
        }
    }

    void Explorer::CopyFileProgress(std::string Path, std::string NewPath, std::function<void(u8 Percentage)> Callback)
    {
        std::string path = this->MakeFull(Path);
        auto ex = GetExplorerForMountName(GetPathRoot(NewPath));
        u64 fsize = this->GetFileSize(path);
        u64 rsize = GetFileSystemOperationsBufferSize();
        u8 *data = GetFileSystemOperationsBuffer();
        u64 szrem = fsize;
        u64 off = 0;
        while(szrem)
        {
            u64 rbytes = this->ReadFileBlock(path, off, std::min(szrem, rsize), data);
            szrem -= rbytes;
            off += rbytes;
            ex->WriteFileBlock(NewPath, data, rbytes);
            u8 perc = ((double)((double)off / (double)fsize) * 100.0);
            Callback(perc);
        }
    }

    void Explorer::CopyDirectory(std::string Dir, std::string NewDir)
    {
        std::string dir = this->MakeFull(Dir);
        std::string ndir = this->MakeFull(NewDir);
        this->CreateDirectory(ndir);
        auto dirs = this->GetDirectories(dir);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            std::string dfrom = dir + "/" + dirs[i];
            std::string dto = ndir + "/" + dirs[i];
            this->CopyDirectory(dfrom, dto);
        }
        auto files = this->GetFiles(dir);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            std::string dfrom = dir + "/" + files[i];
            std::string dto = ndir + "/" + files[i];
            this->CopyFile(dfrom, dto);
        }
    }

    void Explorer::CopyDirectoryProgress(std::string Dir, std::string NewDir, std::function<void(u8 Percentage)> Callback)
    {
        std::string dir = this->MakeFull(Dir);
        std::string ndir = this->MakeFull(NewDir);
        this->CreateDirectory(ndir);
        auto dirs = this->GetDirectories(dir);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            std::string dfrom = dir + "/" + dirs[i];
            std::string dto = ndir + "/" + dirs[i];
            this->CopyDirectoryProgress(dfrom, dto, Callback);
        }
        auto files = this->GetFiles(dir);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            std::string dfrom = dir + "/" + files[i];
            std::string dto = ndir + "/" + files[i];
            this->CopyFileProgress(dfrom, dto, Callback);
        }
    }

    bool Explorer::IsFileBinary(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        if(!this->IsFile(path)) return false;
        bool bin = false;
        u64 fsize = this->GetFileSize(path);
        if(fsize == 0) return true;
        u64 toread = std::min(fsize, (u64)0x200); // 0x200 like GodMode9
        u8 *ptr = GetFileSystemOperationsBuffer();
        u64 rsize = this->ReadFileBlock(path, 0, toread, ptr);
        for(u32 i = 0; i < rsize; i++)
        {
            char ch = (char)ptr[i];
            if(rsize == 0) return true;
            if(!isascii(ch) || (iscntrl(ch) && !isspace(ch)))
            {
                bin = true;
                break;
            }
        }
        return bin;
    }

    std::vector<u8> Explorer::ReadFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        u64 fsize = this->GetFileSize(path);
        if(fsize == 0) return std::vector<u8>();
        u8 *data = (u8*)memalign(0x1000, fsize);
        u64 rsize = this->ReadFileBlock(path, 0, fsize, data);
        if(rsize == 0)
        {
            free(data);
            return std::vector<u8>();
        }
        std::vector<u8> vc(data, data + rsize);
        free(data);
        return vc;
    }

    std::vector<std::string> Explorer::ReadFileLines(std::string Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<std::string> data;
        std::string path = this->MakeFull(Path);
        u64 fsize = this->GetFileSize(path);
        if(fsize == 0) return data;
        std::string tmpline;
        u32 tmpc = 0;
        u32 tmpo = 0;
        u64 szrem = fsize;
        u64 off = 0;
        u8 *tmpdata = GetFileSystemOperationsBuffer();
        bool end = false;
        while(szrem && !end)
        {
            u64 rsize = this->ReadFileBlock(path, off, std::min((u64)0x1000, szrem), tmpdata);
            if(rsize == 0) return data;
            szrem -= rsize;
            off += rsize;
            for(u32 i = 0; i < rsize; i++)
            {
                char ch = (char)tmpdata[i];
                if(ch == '\n')
                {
                    if(tmpc >= LineCount)
                    {
                        end = true;
                        break;
                    }
                    if((tmpo < LineOffset) && (LineOffset != 0))
                    {
                        tmpo++;
                        tmpline = "";
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
                    tmpline = "";
                }
                else tmpline += (char)ch;
            }
        }
        if(!tmpline.empty())
        {
            data.push_back(tmpline);
            tmpline = "";
        }
        return data;
    }

    std::vector<std::string> Explorer::ReadFileFormatHex(std::string Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<std::string> sdata;
        std::string path = this->MakeFull(Path);
        u64 sz = this->GetFileSize(path);
        u64 off = (16 * LineOffset);
        u64 rsz = (16 * LineCount);
        if(off >= sz) return sdata;
        u64 rrsz = std::min(sz, rsz);
        if((off + rsz) > sz) rrsz = rsz - ((off + rsz) - sz);
        std::vector<u8> bdata(rrsz);
        this->ReadFileBlock(path, off, rrsz, bdata.data());
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
        return sdata;
    }

    u64 Explorer::GetDirectorySize(std::string Path)
    {
        u64 sz = 0;
        std::string path = this->MakeFull(Path);
        auto dirs = this->GetDirectories(path);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            std::string pd = path + "/" + dirs[i];
            sz += this->GetDirectorySize(pd);
        }
        auto files = this->GetFiles(path);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            std::string pd = path + "/" + files[i];
            sz += this->GetFileSize(pd);
        }
        return sz;
    }

    void Explorer::DeleteDirectory(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        auto dirs = this->GetDirectories(path);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            std::string pd = path + "/" + dirs[i];
            this->DeleteDirectory(pd);
        }
        auto files = this->GetFiles(path);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            std::string pd = path + "/" + files[i];
            this->DeleteFile(pd);
        }
        this->DeleteDirectorySingle(path);
    }

    bool StdCaseCompare(std::string a, std::string b)
    {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return (a < b);
    }

    std::vector<std::string> StdExplorer::GetDirectories(std::string Path)
    {
        std::vector<std::string> dirs;
        std::string path = this->MakeFull(Path);
        DIR *dp = opendir(path.c_str());
        if(dp)
        {
            struct dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                std::string ent = std::string(dt->d_name);
                if(this->IsDirectory(path + "/" + ent)) dirs.push_back(ent);
            }
        }
        closedir(dp);
        return dirs;
    }

    std::vector<std::string> StdExplorer::GetFiles(std::string Path)
    {
        std::vector<std::string> files;
        std::string path = this->MakeFull(Path);
        DIR *dp = opendir(path.c_str());
        if(dp)
        {
            struct dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                std::string ent = std::string(dt->d_name);
                if(this->IsFile(path + "/" + ent)) files.push_back(ent);
            }
        }
        closedir(dp);
        return files;
    }

    bool StdExplorer::Exists(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        struct stat st;
        return (stat(path.c_str(), &st) == 0);
    }

    bool StdExplorer::IsFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        struct stat st;
        return ((stat(path.c_str(), &st) == 0) && (st.st_mode & S_IFREG));
    }

    bool StdExplorer::IsDirectory(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        struct stat st;
        return ((stat(path.c_str(), &st) == 0) && (st.st_mode & S_IFDIR));
    }
    
    void StdExplorer::CreateFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        std::ofstream nfile(path);
        nfile.close();
    }

    void StdExplorer::CreateDirectory(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        mkdir(path.c_str(), 777);
    }

    void StdExplorer::RenameFile(std::string Path, std::string NewName)
    {
        std::string path = this->MakeFull(Path);
        std::string npath = this->MakeFull(NewName);
        rename(path.c_str(), npath.c_str());
    }

    void StdExplorer::RenameDirectory(std::string Path, std::string NewName)
    {
        return this->RenameFile(Path, NewName);
    }

    void StdExplorer::DeleteFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        remove(path.c_str());
    }

    void StdExplorer::DeleteDirectorySingle(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        rmdir(path.c_str());
    }

    u64 StdExplorer::ReadFileBlock(std::string Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsz = 0;
        std::string path = this->MakeFull(Path);
        FILE *f = fopen(path.c_str(), "rb");
        if(f)
        {
            fseek(f, Offset, SEEK_SET);
            rsz = fread(Out, 1, Size, f);
        }
        fclose(f);
        return rsz;
    }

    u64 StdExplorer::WriteFileBlock(std::string Path, u8 *Data, u64 Size)
    {
        u64 wsz = 0;
        std::string path = this->MakeFull(Path);
        FILE *f = fopen(path.c_str(), "wb");
        if(f)
        {
            wsz = fwrite(Data, 1, Size, f);
            fclose(f);
        }
        return wsz;
    }

    u64 StdExplorer::GetFileSize(std::string Path)
    {
        u64 sz = 0;
        std::string path = this->MakeFull(Path);
        struct stat st;
        if(stat(path.c_str(), &st) == 0) sz = st.st_size;
        return sz;
    }

    u64 StdExplorer::GetTotalSpace()
    {
        return 0;
    }

    u64 StdExplorer::GetFreeSpace()
    {
        return 0;
    }

    SdCardExplorer::SdCardExplorer()
    {
        this->SetNames("sdmc", "SdCard");
    }

    u64 SdCardExplorer::GetTotalSpace()
    {
        u64 sz = 0;
        fsFsGetTotalSpace(fsdevGetDefaultFileSystem(), "/", &sz);
        return sz;
    }

    u64 SdCardExplorer::GetFreeSpace()
    {
        u64 sz = 0;
        fsFsGetFreeSpace(fsdevGetDefaultFileSystem(), "/", &sz);
        return sz;
    }

    NANDExplorer::NANDExplorer(Partition Part)
    {
        this->part = Part;
        switch(Part)
        {
            case Partition::PRODINFOF:
            {
                fsOpenBisFileSystem(&this->fs, FsBisStorageId_CalibrationFile, "");
                fsdevMountDevice("gprodinfof", this->fs);
                this->SetNames("gprodinfof", "ProdInfoF");
                break;
            }
            case Partition::NANDSafe:
            {
                fsOpenBisFileSystem(&this->fs, FsBisStorageId_SafeMode, "");
                fsdevMountDevice("gnsafe", this->fs);
                this->SetNames("gnsafe", "Safe");
                break;
            }
            case Partition::NANDUser:
            {
                fsOpenBisFileSystem(&this->fs, FsBisStorageId_User, "");
                fsdevMountDevice("gnuser", this->fs);
                this->SetNames("gnuser", "User");
                break;
            }
            case Partition::NANDSystem:
            {
                fsOpenBisFileSystem(&this->fs, FsBisStorageId_System, "");
                fsdevMountDevice("gnsystem", this->fs);
                this->SetNames("gnsystem", "System");
                break;
            }
            default:
                break;
        }
    }

    Partition NANDExplorer::GetPartition()
    {
        return this->part;
    }

    bool NANDExplorer::ShouldWarnOnWriteAccess()
    {
        return true;
    }

    u64 NANDExplorer::GetTotalSpace()
    {
        u64 sz = 0;
        fsFsGetTotalSpace(&this->fs, "/", &sz);
        return sz;
    }

    u64 NANDExplorer::GetFreeSpace()
    {
        u64 sz = 0;
        fsFsGetFreeSpace(&this->fs, "/", &sz);
        return sz;
    }

    void NANDExplorer::Close()
    {
        switch(this->part)
        {
            case Partition::PRODINFOF:
            {
                fsdevUnmountDevice("gprodinfof");
                break;
            }
            case Partition::NANDSafe:
            {
                fsdevUnmountDevice("gnsafe");
                break;
            }
            case Partition::NANDUser:
            {
                fsdevUnmountDevice("gnuser");
                break;
            }
            case Partition::NANDSystem:
            {
                fsdevUnmountDevice("gnsystem");
                break;
            }
            default:
                break;
        }
    }

    USBPCDriveExplorer::USBPCDriveExplorer(std::string MountName)
    {
        this->SetNames(MountName, MountName);
    }

    std::vector<std::string> USBPCDriveExplorer::GetDirectories(std::string Path)
    {
        std::vector<std::string> dirs;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::ListDirectories))
        {
            usb::WriteString(path);
            u32 count = usb::Read32();
            if(count > 0) for(u32 i = 0; i < count; i++)
            {
                std::string dir = usb::ReadString();
                dirs.push_back(dir);
            }
        }
        return dirs;
    }

    std::vector<std::string> USBPCDriveExplorer::GetFiles(std::string Path)
    {
        std::vector<std::string> files;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::ListFiles))
        {
            usb::WriteString(path);
            u32 count = usb::Read32();
            if(count > 0) for(u32 i = 0; i < count; i++)
            {
                std::string file = usb::ReadString();
                files.push_back(file);
            }
        }
        return files;
    }

    bool USBPCDriveExplorer::Exists(std::string Path)
    {
        bool ex = false;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::GetPathType))
        {
            usb::WriteString(path);
            u32 type = usb::Read32();
            ex = (type != 0);
        }
        return ex;
    }

    bool USBPCDriveExplorer::IsFile(std::string Path)
    {
        bool ex = false;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::GetPathType))
        {
            usb::WriteString(path);
            u32 type = usb::Read32();
            ex = (type == 1);
        }
        return ex;
    }

    bool USBPCDriveExplorer::IsDirectory(std::string Path)
    {
        bool ex = false;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::GetPathType))
        {
            usb::WriteString(path);
            u32 type = usb::Read32();
            ex = (type == 2);
        }
        return ex;
    }

    void USBPCDriveExplorer::CreateFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::CreateFile)) usb::WriteString(path);
    }

    void USBPCDriveExplorer::CreateDirectory(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::CreateDirectory)) usb::WriteString(path);
    }

    void USBPCDriveExplorer::RenameFile(std::string Path, std::string NewName)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::RenameFile))
        {
            usb::WriteString(path);
            usb::WriteString(NewName);
        }
    }

    void USBPCDriveExplorer::RenameDirectory(std::string Path, std::string NewName)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::RenameDirectory))
        {
            usb::WriteString(path);
            usb::WriteString(NewName);
        }
    }

    void USBPCDriveExplorer::DeleteFile(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::DeleteFile))
        {
            usb::WriteString(path);
        }
    }

    void USBPCDriveExplorer::DeleteDirectorySingle(std::string Path)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::DeleteDirectory))
        {
            usb::WriteString(path);
        }
    }

    u64 USBPCDriveExplorer::ReadFileBlock(std::string Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsize = 0;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::FileRead))
        {
            usb::Write64(Offset);
            usb::Write64(Size);
            usb::WriteString(path);
            rsize = usb::Read64();
            if(rsize > 0) usb::ReadSimple(Out, Size);
        }
        return rsize;
    }

    u64 USBPCDriveExplorer::WriteFileBlock(std::string Path, u8 *Data, u64 Size)
    {
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::FileWrite))
        {
            usb::Write64(0);
            usb::Write64(Size);
            usb::WriteString(path);
            usb::Write(Data, Size);
        }
        return Size;
    }

    u64 USBPCDriveExplorer::GetFileSize(std::string Path)
    {
        u64 sz = 0;
        std::string path = this->MakeFull(Path);
        if(usb::WriteCommandInput(usb::CommandId::GetFileSize))
        {
            usb::WriteString(path);
            sz = usb::Read64();
        }
        return sz;
    }

    u64 USBPCDriveExplorer::GetTotalSpace()
    {
        u64 sz = 0;
        if(usb::WriteCommandInput(usb::CommandId::GetDriveTotalSpace))
        {
            usb::WriteString(this->mntname);
            sz = usb::Read64();
        }
        return sz;
    }

    u64 USBPCDriveExplorer::GetFreeSpace()
    {
        u64 sz = 0;
        if(usb::WriteCommandInput(usb::CommandId::GetDriveFreeSpace))
        {
            usb::WriteString(this->mntname);
            sz = usb::Read64();
        }
        return sz;
    }

    FileSystemExplorer::FileSystemExplorer(std::string MountName, std::string DisplayName, FsFileSystem *FileSystem, bool AutoClose)
    {
        this->fs = FileSystem;
        this->aclose = AutoClose;
        this->SetNames(MountName, DisplayName);
        fsdevMountDevice(MountName.c_str(), *this->fs);
    }

    FsFileSystem *FileSystemExplorer::GetFileSystem()
    {
        return this->fs;
    }

    u64 FileSystemExplorer::GetTotalSpace()
    {
        u64 sz = 0;
        fsFsGetTotalSpace(this->fs, "/", &sz);
        return sz;
    }

    u64 FileSystemExplorer::GetFreeSpace()
    {
        u64 sz = 0;
        fsFsGetFreeSpace(this->fs, "/", &sz);
        return sz;
    }

    void FileSystemExplorer::Close()
    {
        if(this->aclose) fsdevUnmountDevice(this->mntname.c_str());
        else fsdevDeleteDevice(this->mntname.c_str());
    }

    Explorer *GetSdCardExplorer()
    {
        if(esdc == NULL) esdc = new SdCardExplorer();
        return esdc;
    }

    Explorer *GetPRODINFOFExplorer()
    {
        if(eprd == NULL) eprd = new NANDExplorer(Partition::PRODINFOF);
        return eprd;
    }

    Explorer *GetNANDSafeExplorer()
    {
        if(ensf == NULL) ensf = new NANDExplorer(Partition::NANDSafe);
        return ensf;
    }

    Explorer *GetNANDUserExplorer()
    {
        if(enus == NULL) enus = new NANDExplorer(Partition::NANDUser);
        return enus;
    }

    Explorer *GetNANDSystemExplorer()
    {
        if(enss == NULL) enss = new NANDExplorer(Partition::NANDSystem);
        return enss;
    }

    Explorer *GetUSBPCDriveExplorer(std::string MountName)
    {
        std::string mname = fs::GetPathRoot(MountName);
        if(epcdrv == NULL)
        {
            epcdrv = new USBPCDriveExplorer(mname);
            if(MountName != mname)
            {
                std::string pth = fs::GetPathWithoutRoot(MountName);
                pth.erase(0, 1);
                epcdrv->NavigateForward(pth);
            }
        }
        else
        {
            if(epcdrv->GetMountName() != MountName)
            {
                delete epcdrv;
                epcdrv = new USBPCDriveExplorer(mname);
                if(MountName != mname)
                {
                    std::string pth = fs::GetPathWithoutRoot(MountName);
                    pth.erase(0, 1);
                    epcdrv->NavigateForward(pth);
                }
            }
        }
        return epcdrv;
    }

    Explorer *GetExplorerForMountName(std::string MountName)
    {
        Explorer *ex = NULL;
        if(esdc != NULL) if(esdc->GetMountName() == MountName) return esdc;
        if(eprd != NULL) if(eprd->GetMountName() == MountName) return eprd;
        if(ensf != NULL) if(ensf->GetMountName() == MountName) return ensf;
        if(enus != NULL) if(enus->GetMountName() == MountName) return enus;
        if(enss != NULL) if(enss->GetMountName() == MountName) return enss;
        if(epcdrv != NULL) if(epcdrv->GetMountName() == MountName) return epcdrv;
        return ex;
    }
}