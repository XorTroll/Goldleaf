
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

#include <fs/fs_RemotePCExplorer.hpp>
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
    RemotePCExplorer::RemotePCExplorer(String MountName)
    {
        this->SetNames(MountName, MountName);
    }

    std::vector<String> RemotePCExplorer::GetDirectories(String Path)
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

    std::vector<String> RemotePCExplorer::GetFiles(String Path)
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

    bool RemotePCExplorer::Exists(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = ((type == 1) || (type == 2));
        return ex;
    }

    bool RemotePCExplorer::IsFile(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = (type == 1);
        return ex;
    }

    bool RemotePCExplorer::IsDirectory(String Path)
    {
        bool ex = false;
        String path = this->MakeFull(Path);
        u32 type = 0;
        u64 tmpfsz = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(type), usb::Out64(tmpfsz));
        ex = (type == 2);
        return ex;
    }

    void RemotePCExplorer::CreateFile(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(1), usb::InString(path));
    }

    void RemotePCExplorer::CreateDirectory(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Create>(usb::In32(2), usb::InString(path));
    }

    void RemotePCExplorer::RenameFile(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(1), usb::InString(path), usb::InString(NewName));
    }

    void RemotePCExplorer::RenameDirectory(String Path, String NewName)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Rename>(usb::In32(2), usb::InString(path), usb::InString(NewName));
    }

    void RemotePCExplorer::DeleteFile(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(1), usb::InString(path));
    }

    void RemotePCExplorer::DeleteDirectorySingle(String Path)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::Delete>(usb::In32(2), usb::InString(path));
    }

    void RemotePCExplorer::StartFile(String path, FileMode mode)
    {
        String npath = this->MakeFull(path);
        usb::ProcessCommand<usb::CommandId::StartFile>(usb::InString(npath), usb::In32((u32)mode));
    }

    u64 RemotePCExplorer::ReadFileBlock(String Path, u64 Offset, u64 Size, u8 *Out)
    {
        u64 rsize = 0;
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::ReadFile>(usb::InString(path), usb::In64(Offset), usb::In64(Size), usb::Out64(rsize), usb::OutBuffer(Out, Size));
        return rsize;
    }

    u64 RemotePCExplorer::WriteFileBlock(String Path, u8 *Data, u64 Size)
    {
        String path = this->MakeFull(Path);
        usb::ProcessCommand<usb::CommandId::WriteFile>(usb::InString(path), usb::In64(Size), usb::InBuffer(Data, Size));
        return Size;
    }

    void RemotePCExplorer::EndFile(FileMode mode)
    {
        usb::ProcessCommand<usb::CommandId::EndFile>(usb::In32((u32)mode));
    }

    u64 RemotePCExplorer::GetFileSize(String Path)
    {
        u64 sz = 0;
        String path = this->MakeFull(Path);
        u32 tmptype = 0;
        usb::ProcessCommand<usb::CommandId::StatPath>(usb::InString(path), usb::Out32(tmptype), usb::Out64(sz));
        return sz;
    }

    u64 RemotePCExplorer::GetTotalSpace()
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

    u64 RemotePCExplorer::GetFreeSpace()
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

    void RemotePCExplorer::SetArchiveBit(String Path)
    {
        // Non-HOS operating systems don't handle archive bit for what we want, so :P
    }
}