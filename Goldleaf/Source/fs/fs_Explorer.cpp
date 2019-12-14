
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

#include <fs/fs_Explorer.hpp>
#include <usb/usb_Commands.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

namespace fs
{
    static SdCardExplorer *esdc = NULL;
    static NANDExplorer *eprd = NULL;
    static NANDExplorer *ensf = NULL;
    static NANDExplorer *enus = NULL;
    static NANDExplorer *enss = NULL;
    static USBPCDriveExplorer *epcdrv = NULL;

    static bool InternalCaseCompare(String a, String b)
    {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return (a.AsUTF16() < b.AsUTF16());
    }

    bool Explorer::ShouldWarnOnWriteAccess()
    {
        return false;
    }

    void Explorer::SetNames(String MountName, String DisplayName)
    {
        this->dspname = DisplayName;
        this->mntname = MountName;
        this->ecwd = MountName + ":/";
    }

    bool Explorer::NavigateBack()
    {
        if(this->ecwd == (this->mntname + ":/")) return false;
        String parent = this->ecwd.substr(0, this->ecwd.find_last_of("/\\"));
        if(parent.substr(parent.length() - 1) == ":") parent += "/";
        this->ecwd = parent;
        return true;
    }

    bool Explorer::NavigateForward(String Path)
    {
        bool idir = this->IsDirectory(Path);
        if(idir) this->ecwd = this->MakeFull(Path);
        return idir;
    }

    std::vector<String> Explorer::GetContents()
    {
        auto dirs = this->GetDirectories(this->ecwd);
        auto files = this->GetFiles(this->ecwd);

        if(!dirs.empty()) std::sort(dirs.begin(), dirs.end(), InternalCaseCompare);
        if(!files.empty())
        {
            std::sort(files.begin(), files.end(), InternalCaseCompare);
            dirs.insert(dirs.end(), files.begin(), files.end());
        }

        return dirs;
    }

    String Explorer::GetMountName()
    {
        return this->mntname;
    }

    String Explorer::GetCwd()
    {
        return this->ecwd;
    }

    String Explorer::GetPresentableCwd()
    {
        if(this->ecwd == (this->mntname + ":/")) return this->dspname + ":/";
        u32 mntrootsize = this->mntname.length() + 2;
        String cwdnoroot = this->ecwd.substr(mntrootsize);
        return this->dspname + ":/" + cwdnoroot;
    }

    void Explorer::CopyFile(String Path, String NewPath)
    {
        String path = this->MakeFull(Path);
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

    void Explorer::CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback)
    {
        String path = this->MakeFull(Path);
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
            Callback((double)off, (double)fsize);
        }
    }

    void Explorer::CopyDirectory(String Dir, String NewDir)
    {
        String dir = this->MakeFull(Dir);
        String ndir = this->MakeFull(NewDir);
        this->CreateDirectory(ndir);
        auto dirs = this->GetDirectories(dir);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            String dfrom = dir + "/" + dirs[i];
            String dto = ndir + "/" + dirs[i];
            this->CopyDirectory(dfrom, dto);
        }
        auto files = this->GetFiles(dir);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            String dfrom = dir + "/" + files[i];
            String dto = ndir + "/" + files[i];
            this->CopyFile(dfrom, dto);
        }
    }

    void Explorer::CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback)
    {
        String dir = this->MakeFull(Dir);
        String ndir = this->MakeFull(NewDir);
        this->CreateDirectory(ndir);
        auto dirs = this->GetDirectories(dir);
        for(auto &cdir: dirs) this->CopyDirectoryProgress(dir + "/" + cdir, ndir + "/" + cdir, Callback);
        auto files = this->GetFiles(dir);
        for(auto &cfile: files) this->CopyFileProgress(dir + "/" + cfile, ndir + "/" + cfile, Callback);
    }

    bool Explorer::IsFileBinary(String Path)
    {
        String path = this->MakeFull(Path);
        if(!this->IsFile(path)) return false;
        bool bin = false;
        u64 fsize = this->GetFileSize(path);
        if(fsize == 0) return true;
        u64 toread = std::min(fsize, (u64)0x200); // 0x200, like GodMode9
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

    std::vector<u8> Explorer::ReadFile(String Path)
    {
        String path = this->MakeFull(Path);
        u64 fsize = this->GetFileSize(path);
        std::vector<u8> data;
        if(fsize == 0) return data;
        data.reserve(fsize);
        this->ReadFileBlock(path, 0, fsize, data.data());
        return data;
    }

    std::vector<String> Explorer::ReadFileLines(String Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<String> data;
        String path = this->MakeFull(Path);
        u64 fsize = this->GetFileSize(path);
        if(fsize == 0) return data;
        String tmpline;
        u32 tmpc = 0;
        u32 tmpo = 0;
        u64 szrem = fsize;
        u64 off = 0;
        u8 *tmpdata = GetFileSystemOperationsBuffer();
        bool end = false;
        while(szrem && !end)
        {
            u64 rsize = this->ReadFileBlock(path, off, std::min((u64)GetFileSystemOperationsBufferSize(), szrem), tmpdata);
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
                    String tab = "\t";
                    while(true)
                    {
                        size_t spos = tmpline.find(tab);
                        if(spos == String::npos) break;
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

    std::vector<String> Explorer::ReadFileFormatHex(String Path, u32 LineOffset, u32 LineCount)
    {
        std::vector<String> sdata;
        String path = this->MakeFull(Path);
        u64 sz = this->GetFileSize(path);
        u64 off = 16 * LineOffset;
        u64 rsz = 16 * LineCount;
        if(off >= sz) return sdata;
        u64 rrsz = std::min(sz, rsz);
        if((off + rsz) > sz) rrsz = rsz - ((off + rsz) - sz);
        std::vector<u8> bdata(rrsz);
        this->ReadFileBlock(path, off, rrsz, bdata.data());
        u32 count = 0;
        String tmpline;
        String tmpchr;
        u32 toff = 0;
        for(u32 i = 0; i < (rrsz + 1); i++)
        {
            if(count == 16)
            {
                std::stringstream ostrm;
                ostrm << std::hex << std::setw(8) << std::uppercase << std::setfill('0') << (off + toff);
                String def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
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
                String def = " " + ostrm.str() + "   " + tmpline + "  " + tmpchr;
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

    u64 Explorer::GetDirectorySize(String Path)
    {
        u64 sz = 0;
        String path = this->MakeFull(Path);
        auto dirs = this->GetDirectories(path);
        for(auto &dir: dirs) sz += this->GetDirectorySize(path + "/" + dir);
        auto files = this->GetFiles(path);
        for(auto &file: files) sz += this->GetFileSize(path + "/" + file);
        return sz;
    }

    void Explorer::DeleteDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        auto dirs = this->GetDirectories(path);
        if(!dirs.empty()) for(u32 i = 0; i < dirs.size(); i++)
        {
            String pd = path + "/" + dirs[i];
            this->DeleteDirectory(pd);
        }
        auto files = this->GetFiles(path);
        if(!files.empty()) for(u32 i = 0; i < files.size(); i++)
        {
            String pd = path + "/" + files[i];
            this->DeleteFile(pd);
        }
        this->DeleteDirectorySingle(path);
    }

    std::vector<String> StdExplorer::GetDirectories(String Path)
    {
        std::vector<String> dirs;
        String path = this->MakeFull(Path);
        DIR *dp = opendir(path.AsUTF8().c_str());
        if(dp)
        {
            struct dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                std::string ent = dt->d_name;
                if(this->IsDirectory(path + "/" + ent)) dirs.push_back(ent);
            }
            closedir(dp);
        }
        return dirs;
    }

    std::vector<String> StdExplorer::GetFiles(String Path)
    {
        std::vector<String> files;
        String path = this->MakeFull(Path);
        DIR *dp = opendir(path.AsUTF8().c_str());
        if(dp)
        {
            struct dirent *dt;
            while(true)
            {
                dt = readdir(dp);
                if(dt == NULL) break;
                std::string ent = dt->d_name;
                if(this->IsFile(path + "/" + ent)) files.push_back(ent);
            }
            closedir(dp);
        }
        return files;
    }

    bool StdExplorer::Exists(String Path)
    {
        String path = this->MakeFull(Path);
        struct stat st;
        return (stat(path.AsUTF8().c_str(), &st) == 0);
    }

    bool StdExplorer::IsFile(String Path)
    {
        String path = this->MakeFull(Path);
        struct stat st;
        return ((stat(path.AsUTF8().c_str(), &st) == 0) && (st.st_mode & S_IFREG));
    }

    bool StdExplorer::IsDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        struct stat st;
        return ((stat(path.AsUTF8().c_str(), &st) == 0) && (st.st_mode & S_IFDIR));
    }
    
    void StdExplorer::CreateFile(String Path)
    {
        String path = this->MakeFull(Path);
        fsdevCreateFile(Path.AsUTF8().c_str(), 0, 0);
    }

    void StdExplorer::CreateDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        mkdir(path.AsUTF8().c_str(), 777);
    }

    void StdExplorer::RenameFile(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        String npath = this->MakeFull(NewName);
        rename(path.AsUTF8().c_str(), npath.AsUTF8().c_str());
    }

    void StdExplorer::RenameDirectory(String Path, String NewName)
    {
        return this->RenameFile(Path, NewName);
    }

    void StdExplorer::DeleteFile(String Path)
    {
        String path = this->MakeFull(Path);
        remove(path.AsUTF8().c_str());
    }

    void StdExplorer::DeleteDirectorySingle(String Path)
    {
        String path = this->MakeFull(Path);
        fsdevDeleteDirectoryRecursively(path.AsUTF8().c_str());
    }

    u64 StdExplorer::ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsz = 0;
        String path = this->MakeFull(Path);
        FILE *f = fopen(path.AsUTF8().c_str(), "rb");
        if(f)
        {
            fseek(f, Offset, SEEK_SET);
            rsz = fread(Out, 1, Size, f);
            fclose(f);
        }
        return rsz;
    }

    u64 StdExplorer::WriteFileBlock(String Path, u8 *Data, u64 Size)
    {
        u64 wsz = 0;
        String path = this->MakeFull(Path);
        FILE *f = fopen(path.AsUTF8().c_str(), "ab+");
        if(f)
        {
            wsz = fwrite(Data, 1, Size, f);
            fclose(f);
        }
        return wsz;
    }

    u64 StdExplorer::GetFileSize(String Path)
    {
        u64 sz = 0;
        String path = this->MakeFull(Path);
        struct stat st;
        if(stat(path.AsUTF8().c_str(), &st) == 0) sz = st.st_size;
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

    void StdExplorer::SetArchiveBit(String Path)
    {
        String path = this->MakeFull(Path);
        fsdevSetConcatenationFileAttribute(path.AsUTF8().c_str());
    }

    SdCardExplorer::SdCardExplorer()
    {
        this->SetNames("sdmc", "SdCard");
    }

    u64 SdCardExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(fsdevGetDeviceFileSystem("sdmc"), "/", &sz);
        return (u64)sz;
    }

    u64 SdCardExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(fsdevGetDeviceFileSystem("sdmc"), "/", &sz);
        return (u64)sz;
    }

    NANDExplorer::NANDExplorer(Partition Part)
    {
        this->part = Part;
        switch(Part)
        {
            case Partition::PRODINFOF:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_CalibrationFile, "");
                fsdevMountDevice("gprodinfof", this->fs);
                this->SetNames("gprodinfof", "ProdInfoF");
                break;
            }
            case Partition::NANDSafe:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_SafeMode, "");
                fsdevMountDevice("gnsafe", this->fs);
                this->SetNames("gnsafe", "Safe");
                break;
            }
            case Partition::NANDUser:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_User, "");
                fsdevMountDevice("gnuser", this->fs);
                this->SetNames("gnuser", "User");
                break;
            }
            case Partition::NANDSystem:
            {
                fsOpenBisFileSystem(&this->fs, FsBisPartitionId_System, "");
                fsdevMountDevice("gnsystem", this->fs);
                this->SetNames("gnsystem", "System");
                break;
            }
            default:
                break;
        }
    }

    NANDExplorer::~NANDExplorer()
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
        s64 sz = 0;
        fsFsGetTotalSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    u64 NANDExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    USBPCDriveExplorer::USBPCDriveExplorer(String MountName)
    {
        this->SetNames(MountName, MountName);
    }

    std::vector<String> USBPCDriveExplorer::GetDirectories(String Path)
    {
        std::vector<String> dirs;
        String path = this->MakeFull(Path);
        u32 dircount = 0;
        auto rc = usb::ProcessCommand<usb::CommandId::GetDirectoryCount>(usb::InString(path), usb::Out32(dircount));
        if(R_SUCCEEDED(rc))
        {
            for(u32 i = 0; i < dircount; i++)
            {
                String dir;
                rc = usb::ProcessCommand<usb::CommandId::GetDirectory>(usb::InString(path), usb::In32(i), usb::OutString(dir));
                if(R_SUCCEEDED(rc)) dirs.push_back(dir);
            }
        }
        return dirs;
    }

    std::vector<String> USBPCDriveExplorer::GetFiles(String Path)
    {
        std::vector<String> files;
        String path = this->MakeFull(Path);
        u32 filecount = 0;
        auto rc = usb::ProcessCommand<usb::CommandId::GetFileCount>(usb::InString(path), usb::Out32(filecount));
        if(R_SUCCEEDED(rc))
        {
            for(u32 i = 0; i < filecount; i++)
            {
                String file;
                rc = usb::ProcessCommand<usb::CommandId::GetFile>(usb::InString(path), usb::In32(i), usb::OutString(file));
                if(R_SUCCEEDED(rc)) files.push_back(file);
            }
        }
        return files;
    }

    bool USBPCDriveExplorer::Exists(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = ((type == 1) || (type == 2));
        return ex;
    }

    bool USBPCDriveExplorer::IsFile(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = (type == 1);
        return ex;
    }

    bool USBPCDriveExplorer::IsDirectory(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = (type == 2);
        return ex;
    }

    void USBPCDriveExplorer::CreateFile(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(1), usb::InString(path));
    }

    void USBPCDriveExplorer::CreateDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(2), usb::InString(path));
    }

    void USBPCDriveExplorer::RenameFile(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(1), usb::InString(path), usb::InString(NewName));
    }

    void USBPCDriveExplorer::RenameDirectory(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(2), usb::InString(path), usb::InString(NewName));
    }

    void USBPCDriveExplorer::DeleteFile(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(1), usb::InString(path));
    }

    void USBPCDriveExplorer::DeleteDirectorySingle(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(2), usb::InString(path));
    }

    u64 USBPCDriveExplorer::ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsize = 0;
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::ReadFile>(usb::InString(path), usb::In64(Offset), usb::In64(Size), usb::Out64(rsize), usb::OutBuffer(Out, Size));
        return rsize;
    }

    u64 USBPCDriveExplorer::WriteFileBlock(String Path, u8 *Data, u64 Size)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::WriteFile>(usb::InString(path), usb::In64(Size), usb::InBuffer(Data, Size));
        return Size;
    }

    u64 USBPCDriveExplorer::GetFileSize(String Path)
    {
        u64 sz = 0;
        String path = this->MakeFull(Path);
        u32 tmptype = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(tmptype), usb::Out64(sz));
        return sz;
    }

    u64 USBPCDriveExplorer::GetTotalSpace()
    {
        u64 sz = 0;
        /*
        if(usb::WriteCommandInput(usb::CommandId::GetDriveTotalSpace))
        {
            usb::WriteString(this->mntname);
            if(!usb::Read64(sz)){}
        }
        */
        return sz;
    }

    u64 USBPCDriveExplorer::GetFreeSpace()
    {
        u64 sz = 0;
        /*
        if(usb::WriteCommandInput(usb::CommandId::GetDriveFreeSpace))
        {
            usb::WriteString(this->mntname);
            if(!usb::Read64(sz)){}
        }
        */
        return sz;
    }

    void USBPCDriveExplorer::SetArchiveBit(String Path)
    {
        // Non-HOS operating systems don't handle archive bit for what we want, so :P
    }

    FileSystemExplorer::FileSystemExplorer(String MountName, String DisplayName, FsFileSystem *FileSystem)
    {
        this->fs = FileSystem;
        this->SetNames(MountName, DisplayName);
        fsdevMountDevice(MountName.AsUTF8().c_str(), *this->fs);
    }

    FileSystemExplorer::~FileSystemExplorer()
    {
        fsdevUnmountDevice(this->mntname.AsUTF8().c_str());
    }

    FsFileSystem *FileSystemExplorer::GetFileSystem()
    {
        return this->fs;
    }

    u64 FileSystemExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(this->fs, "/", &sz);
        return (u64)sz;
    }

    u64 FileSystemExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(this->fs, "/", &sz);
        return (u64)sz;
    }

    SdCardExplorer *GetSdCardExplorer()
    {
        if(esdc == NULL) esdc = new SdCardExplorer();
        return esdc;
    }

    NANDExplorer *GetPRODINFOFExplorer()
    {
        if(eprd == NULL) eprd = new NANDExplorer(Partition::PRODINFOF);
        return eprd;
    }

    NANDExplorer *GetNANDSafeExplorer()
    {
        if(ensf == NULL) ensf = new NANDExplorer(Partition::NANDSafe);
        return ensf;
    }

    NANDExplorer *GetNANDUserExplorer()
    {
        if(enus == NULL) enus = new NANDExplorer(Partition::NANDUser);
        return enus;
    }

    NANDExplorer *GetNANDSystemExplorer()
    {
        if(enss == NULL) enss = new NANDExplorer(Partition::NANDSystem);
        return enss;
    }

    USBPCDriveExplorer *GetUSBPCDriveExplorer(String MountName)
    {
        String mname = fs::GetPathRoot(MountName);
        if(epcdrv == NULL)
        {
            epcdrv = new USBPCDriveExplorer(mname);
            if(MountName != mname)
            {
                String pth = fs::GetPathWithoutRoot(MountName);
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
                    String pth = fs::GetPathWithoutRoot(MountName);
                    pth.erase(0, 1);
                    epcdrv->NavigateForward(pth);
                }
            }
        }
        return epcdrv;
    }

    Explorer *GetExplorerForMountName(String MountName)
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

    Explorer *GetExplorerForPath(String Path)
    {
        return GetExplorerForMountName(GetPathRoot(Path));
    }
}