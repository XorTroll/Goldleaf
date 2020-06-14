
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <fs/fs_StdExplorer.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

namespace fs
{
    StdExplorer::StdExplorer() : r_file_obj(nullptr), w_file_obj(nullptr)
    {
        this->commit_fn = [](){};
    }

    void StdExplorer::SetCommitFunction(std::function<void()> fn)
    {
        this->commit_fn = fn;
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
                if(dt == nullptr) break;
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
                if(dt == nullptr) break;
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
        this->commit_fn();
    }

    void StdExplorer::CreateDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        mkdir(path.AsUTF8().c_str(), 777);
        this->commit_fn();
    }

    void StdExplorer::RenameFile(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        String npath = this->MakeFull(NewName);
        rename(path.AsUTF8().c_str(), npath.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::RenameDirectory(String Path, String NewName)
    {
        return this->RenameFile(Path, NewName);
    }

    void StdExplorer::DeleteFile(String Path)
    {
        String path = this->MakeFull(Path);
        remove(path.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::DeleteDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        fsdevDeleteDirectoryRecursively(path.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::StartFile(String path, FileMode mode)
    {
        auto fmode = "rw";
        switch(mode)
        {
            case FileMode::Read:
                fmode = "rb";
                break;
            case FileMode::Write:
                fmode = "wb";
                break;
            case FileMode::Append:
                fmode = "ab+";
                break;
        }
        this->EndFile(mode);
        String npath = this->MakeFull(path);
        if(mode == FileMode::Read) this->r_file_obj = fopen(npath.AsUTF8().c_str(), fmode);
        else this->w_file_obj = fopen(npath.AsUTF8().c_str(), fmode);
    }

    u64 StdExplorer::ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsz = 0;

        if(this->r_file_obj != nullptr)
        {
            fseek(this->r_file_obj, Offset, SEEK_SET);
            rsz = fread(Out, 1, Size, this->r_file_obj);
            return rsz;
        }

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
        

        if(this->w_file_obj != nullptr)
        {
            wsz = fwrite(Data, 1, Size, this->w_file_obj);
            return wsz;
        }

        String path = this->MakeFull(Path);

        FILE *f = fopen(path.AsUTF8().c_str(), "ab+");
        if(f)
        {
            wsz = fwrite(Data, 1, Size, f);
            fclose(f);
        }
        return wsz;
    }

    void StdExplorer::EndFile(FileMode mode)
    {
        if(mode == FileMode::Read)
        {
            if(this->r_file_obj != nullptr)
            {
                fclose(this->r_file_obj);
                this->r_file_obj = nullptr;
            }
        }
        else
        {
            if(this->w_file_obj != nullptr)
            {
                fclose(this->w_file_obj);
                this->commit_fn();
                this->w_file_obj = nullptr;
            }
        }
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
        this->commit_fn();
    }
}