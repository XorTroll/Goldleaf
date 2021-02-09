
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

#include <fs/fs_FspExplorers.hpp>
#include <hos/hos_Common.hpp>
#include <hos/hos_Titles.hpp>

namespace fs
{
    static std::vector<std::string> mount_name_table;

    static std::string AllocateMountName()
    {
        auto id = randomGet64();
        std::string name = "gmount-" + hos::FormatHex(id);
        for(auto &mount_name: mount_name_table)
        {
            if(name == mount_name)
            {
                // Continue trying random numbers until we find an unused one
                return AllocateMountName();
            }
        }
        mount_name_table.push_back(name);
        return name;
    }

    static void DisposeMountName(std::string name)
    {
        if(name.substr(0, 7) != "gmount-") return;
        u32 idx = 0;
        bool found = false;
        for(auto &mount_name: mount_name_table)
        {
            if(name == mount_name)
            {
                found = true;
                break;
            }
            idx++;
        }
        if(found)
        {
            mount_name_table.erase(mount_name_table.begin() + idx);
        }
    }

    FspExplorer::FspExplorer(String DisplayName, FsFileSystem FileSystem) : StdExplorer(), dispose(true)
    {
        this->fs = FileSystem;
        auto mount_name = AllocateMountName();
        this->SetNames(mount_name, DisplayName);
        fsdevMountDevice(this->mntname.AsUTF8().c_str(), this->fs);
    }

    FspExplorer::FspExplorer(String DisplayName, std::string mount_name, FsFileSystem FileSystem) : StdExplorer(), dispose(false)
    {
        this->fs = FileSystem;
        this->SetNames(mount_name, DisplayName);
    }

    FspExplorer::~FspExplorer()
    {
        if(this->dispose)
        {
            fsdevUnmountDevice(this->mntname.AsUTF8().c_str());
            DisposeMountName(this->mntname.AsUTF8());
        }
    }

    bool FspExplorer::IsOk()
    {
        return serviceIsActive(&this->fs.s);
    }

    FsFileSystem *FspExplorer::GetFileSystem()
    {
        return &this->fs;
    }

    u64 FspExplorer::GetTotalSpace()
    {
        s64 sz = 0;
        fsFsGetTotalSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    u64 FspExplorer::GetFreeSpace()
    {
        s64 sz = 0;
        fsFsGetFreeSpace(&this->fs, "/", &sz);
        return (u64)sz;
    }

    SdCardExplorer::SdCardExplorer() : FspExplorer("SdCard", "sdmc", *fsdevGetDeviceFileSystem("sdmc"))
    {
    }

    NANDExplorer::NANDExplorer(Partition Part) : FspExplorer(NANDExplorer::GetNANDPartitionName(Part), NANDExplorer::MountNANDFileSystem(Part)), part(Part)
    {
        this->SetShouldWarnOnWriteAccess(true);
    }

    std::string NANDExplorer::GetNANDPartitionName(Partition part)
    {
        switch(part)
        {
            case Partition::PRODINFOF:
                return "PRODINFOF";
            case Partition::NANDSafe:
                return "NAND-Safe";
            case Partition::NANDUser:
                return "NAND-User";
            case Partition::NANDSystem:
                return "NAND-System";
            default:
                break;
        }
        return "NAND";
    }

    FsFileSystem NANDExplorer::MountNANDFileSystem(Partition part)
    {
        FsFileSystem fs;
        FsBisPartitionId id;
        switch(part)
        {
            case Partition::PRODINFOF:
            {
                id = FsBisPartitionId_CalibrationFile;
                break;
            }
            case Partition::NANDSafe:
            {
                id = FsBisPartitionId_SafeMode;
                break;
            }
            case Partition::NANDUser:
            {
                id = FsBisPartitionId_User;
                break;
            }
            case Partition::NANDSystem:
            {
                id = FsBisPartitionId_System;
                break;
            }
            default:
                // Return empty filesystem
                return fs;
        }
        fsOpenBisFileSystem(&fs, id, "");
        return fs;
    }

    Partition NANDExplorer::GetPartition()
    {
        return this->part;
    }

    TitleSaveDataExplorer::TitleSaveDataExplorer(u64 app_id, AccountUid user_id) : FspExplorer(std::string("SaveData-0x") + hos::FormatApplicationId(app_id), TitleSaveDataExplorer::MountTitleSaveData(app_id, user_id)), appid(app_id), uid(user_id)
    {
        this->SetCommitFunction(std::bind(&TitleSaveDataExplorer::DoCommit, this));
    }

    FsFileSystem TitleSaveDataExplorer::MountTitleSaveData(u64 app_id, AccountUid user_id)
    {
        FsFileSystem fs;

        FsSaveDataAttribute attr = {};
        attr.application_id = app_id;
        attr.uid = user_id;
        attr.save_data_type = FsSaveDataType_Account;

        fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);
        return fs;
    }

    void TitleSaveDataExplorer::DoCommit()
    {
        fsdevCommitDevice(this->mntname.AsUTF8().c_str());
    }

    bool TitleSaveDataExplorer::Matches(u64 app_id, AccountUid user_id)
    {
        if(this->appid == app_id)
        {
            if(memcmp(user_id.uid, this->uid.uid, sizeof(this->uid)) == 0)
            {
                return true;
            }
        }
        return false;
    }
}