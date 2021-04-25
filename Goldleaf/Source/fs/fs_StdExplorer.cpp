
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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
#include <dirent.h>

namespace fs {

    StdExplorer::StdExplorer() : r_file_obj(nullptr), w_file_obj(nullptr) {
        this->commit_fn = [](){};
    }

    void StdExplorer::SetCommitFunction(std::function<void()> fn) {
        this->commit_fn = fn;
    }

    std::vector<String> StdExplorer::GetDirectories(String path) {
        std::vector<String> dirs;
        auto full_path = this->MakeFull(path);
        auto dp = opendir(full_path.AsUTF8().c_str());
        if(dp) {
            while(true) {
                auto dt = readdir(dp);
                if(dt == nullptr) {
                    break;
                }
                if(dt->d_type & DT_DIR) {
                    dirs.push_back(dt->d_name);
                }
            }
            closedir(dp);
        }
        return dirs;
    }

    std::vector<String> StdExplorer::GetFiles(String path)
    {
        std::vector<String> files;
        auto full_path = this->MakeFull(path);
        auto dp = opendir(full_path.AsUTF8().c_str());
        if(dp) {
            while(true) {
                auto dt = readdir(dp);
                if(dt == nullptr) {
                    break;
                }
                if(dt->d_type & DT_REG) {
                    files.push_back(dt->d_name);
                }
            }
            closedir(dp);
        }
        return files;
    }

    bool StdExplorer::Exists(String path) {
        auto full_path = this->MakeFull(path);
        struct stat st;
        return stat(full_path.AsUTF8().c_str(), &st) == 0;
    }

    bool StdExplorer::IsFile(String path) {
        auto full_path = this->MakeFull(path);
        struct stat st;
        return (stat(full_path.AsUTF8().c_str(), &st) == 0) && (st.st_mode & S_IFREG);
    }

    bool StdExplorer::IsDirectory(String path) {
        auto full_path = this->MakeFull(path);
        struct stat st;
        return (stat(full_path.AsUTF8().c_str(), &st) == 0) && (st.st_mode & S_IFDIR);
    }
    
    void StdExplorer::CreateFile(String path) {
        auto full_path = this->MakeFull(path);
        fsdevCreateFile(full_path.AsUTF8().c_str(), 0, 0);
        this->commit_fn();
    }

    void StdExplorer::CreateDirectory(String path) {
        auto full_path = this->MakeFull(path);
        mkdir(full_path.AsUTF8().c_str(), 777);
        this->commit_fn();
    }

    void StdExplorer::RenameFile(String path, String NewName) {
        auto full_path = this->MakeFull(path);
        auto full_new_path = this->MakeFull(NewName);
        rename(full_path.AsUTF8().c_str(), full_new_path.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::RenameDirectory(String path, String NewName) {
        return this->RenameFile(path, NewName);
    }

    void StdExplorer::DeleteFile(String path) {
        auto full_path = this->MakeFull(path);
        remove(full_path.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::DeleteDirectory(String path) {
        auto full_path = this->MakeFull(path);
        fsdevDeleteDirectoryRecursively(full_path.AsUTF8().c_str());
        this->commit_fn();
    }

    void StdExplorer::StartFileImpl(String path, FileMode mode) {
        auto file_mode = "rw";
        switch(mode) {
            case FileMode::Read: {
                file_mode = "rb";
                break;
            }
            case FileMode::Write: {
                file_mode = "wb";
                break;
            }
            case FileMode::Append: {
                file_mode = "ab+";
                break;
            }
        }

        auto full_path = this->MakeFull(path);
        if(mode == FileMode::Read) {
            this->r_file_obj = fopen(full_path.AsUTF8().c_str(), file_mode);
        }
        else {
            this->w_file_obj = fopen(full_path.AsUTF8().c_str(), file_mode);
        }
    }

    void StdExplorer::EndFileImpl(FileMode mode) {
        switch(mode) {
            case FileMode::Read: {
                if(this->r_file_obj != nullptr) {
                    fclose(this->r_file_obj);
                    this->r_file_obj = nullptr;
                }
                break;
            }
            default: {
                if(this->w_file_obj != nullptr) {
                    fclose(this->w_file_obj);
                    this->w_file_obj = nullptr;
                    this->commit_fn();
                }
                break;
            }
        }
    }

    u64 StdExplorer::ReadFile(String path, u64 offset, u64 size, void *read_buf) {
        if(this->r_file_obj != nullptr) {
            fseek(this->r_file_obj, offset, SEEK_SET);
            return fread(read_buf, 1, size, this->r_file_obj);
        }

        u64 read_size = 0;
        auto full_path = this->MakeFull(path);
        auto f = fopen(full_path.AsUTF8().c_str(), "rb");
        if(f) {
            fseek(f, offset, SEEK_SET);
            read_size = fread(read_buf, 1, size, f);
            fclose(f);
        }
        return read_size;
    }

    u64 StdExplorer::WriteFile(String path, const void *write_buf, u64 size) {
        if(this->w_file_obj != nullptr) {
            return fwrite(write_buf, 1, size, this->w_file_obj);
        }

        u64 write_size = 0;
        auto full_path = this->MakeFull(path);
        auto f = fopen(full_path.AsUTF8().c_str(), "ab+");
        if(f) {
            write_size = fwrite(write_buf, 1, size, f);
            fclose(f);
        }
        return write_size;
    }

    u64 StdExplorer::GetFileSize(String path) {
        u64 file_size = 0;
        auto full_path = this->MakeFull(path);

        struct stat st;
        if(stat(full_path.AsUTF8().c_str(), &st) == 0) {
            file_size = st.st_size;
        }
        return file_size;
    }

    u64 StdExplorer::GetTotalSpace() {
        // TODO?
        return 0;
    }

    u64 StdExplorer::GetFreeSpace() {
        // TODO?
        return 0;
    }

    void StdExplorer::SetArchiveBit(String path) {
        auto full_path = this->MakeFull(path);
        fsdevSetConcatenationFileAttribute(full_path.AsUTF8().c_str());
        this->commit_fn();
    }

}