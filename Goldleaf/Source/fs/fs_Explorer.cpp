
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
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

namespace fs
{
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
        auto ex = GetExplorerForPath(NewPath);
        String npath = ex->MakeFull(NewPath);
        u64 fsize = this->GetFileSize(path);
        u64 rsize = GetFileSystemOperationsBufferSize();
        u8 *data = GetFileSystemOperationsBuffer();
        u64 szrem = fsize;
        u64 off = 0;
        this->StartFile(path, fs::FileMode::Read);
        ex->StartFile(npath, fs::FileMode::Write);
        while(szrem)
        {
            u64 rbytes = this->ReadFileBlock(path, off, std::min(szrem, rsize), data);
            szrem -= rbytes;
            off += rbytes;
            ex->WriteFileBlock(NewPath, data, rbytes);
        }
        this->EndFile(fs::FileMode::Read);
        ex->EndFile(fs::FileMode::Write);
    }

    void Explorer::CopyFileProgress(String Path, String NewPath, std::function<void(double Done, double Total)> Callback)
    {
        String path = this->MakeFull(Path);
        auto ex = GetExplorerForPath(NewPath);
        String npath = ex->MakeFull(NewPath);
        u64 fsize = this->GetFileSize(path);
        u64 rsize = GetFileSystemOperationsBufferSize();
        u8 *data = GetFileSystemOperationsBuffer();
        u64 szrem = fsize;
        u64 off = 0;
        this->StartFile(path, fs::FileMode::Read);
        ex->StartFile(npath, fs::FileMode::Write);
        while(szrem)
        {
            u64 rbytes = this->ReadFileBlock(path, off, std::min(szrem, rsize), data);
            szrem -= rbytes;
            off += rbytes;
            ex->WriteFileBlock(npath, data, rbytes);
            Callback((double)off, (double)fsize);
        }
        this->EndFile(fs::FileMode::Read);
        ex->EndFile(fs::FileMode::Write);
    }

    void Explorer::CopyDirectory(String Dir, String NewDir)
    {
        String dir = this->MakeFull(Dir);
        auto ex = GetExplorerForPath(NewDir);
        String ndir = ex->MakeFull(NewDir);
        ex->CreateDirectory(ndir);
        auto dirs = this->GetDirectories(dir);
        for(auto &qdir: dirs)
        {
            String dfrom = dir + "/" + qdir;
            String dto = ndir + "/" + qdir;
            this->CopyDirectory(dfrom, dto);
        }
        auto files = this->GetFiles(dir);
        for(auto &qfile: files)
        {
            String dfrom = dir + "/" + qfile;
            String dto = ndir + "/" + qfile;
            this->CopyFile(dfrom, dto);
        }
    }

    void Explorer::CopyDirectoryProgress(String Dir, String NewDir, std::function<void(double Done, double Total)> Callback)
    {
        String dir = this->MakeFull(Dir);
        auto ex = GetExplorerForPath(NewDir);
        String ndir = ex->MakeFull(NewDir);
        ex->CreateDirectory(ndir);
        auto files = this->GetFiles(dir);
        for(auto &cfile: files) this->CopyFileProgress(dir + "/" + cfile, ndir + "/" + cfile, Callback);
        auto dirs = this->GetDirectories(dir);
        for(auto &cdir: dirs) this->CopyDirectoryProgress(dir + "/" + cdir, ndir + "/" + cdir, Callback);
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
        for(auto &dir: dirs)
        {
            String pd = path + "/" + dir;
            this->DeleteDirectory(pd);
        }
        auto files = this->GetFiles(path);
        for(auto &file: files)
        {
            String pd = path + "/" + file;
            this->DeleteFile(pd);
        }
        this->DeleteDirectorySingle(path);
    }
}