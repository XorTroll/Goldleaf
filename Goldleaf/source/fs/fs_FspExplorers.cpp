
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

#include <fs/fs_FspExplorers.hpp>
#include <hos/hos_Common.hpp>
#include <hos/hos_Titles.hpp>
#include <acc/acc_UserSelection.hpp>

namespace fs {

    namespace {

        constexpr const char *MountNamePrefix = "gmount-";

        std::vector<std::string> g_MountNameTable;

        std::string AllocateMountName() {
            const auto id = randomGet64();
            const auto &name = MountNamePrefix + hos::FormatHex(id);
            for(const auto &mount_name: g_MountNameTable) {
                if(name == mount_name) {
                    // Continue trying random numbers until we find an unused one
                    return AllocateMountName();
                }
            }
            g_MountNameTable.push_back(name);
            return name;
        }

        void DisposeMountName(std::string name) {
            if(name.substr(0, 7) != MountNamePrefix) {
                return;
            }
            u32 idx = 0;
            bool found = false;
            for(const auto &mount_name: g_MountNameTable) {
                if(name == mount_name) {
                    found = true;
                    break;
                }
                idx++;
            }
            if(found) {
                g_MountNameTable.erase(g_MountNameTable.begin() + idx);
            }
        }

    }

    FspExplorer::FspExplorer(String display_name, FsFileSystem file_system) : StdExplorer(), dispose(true) {
        this->fs = file_system;
        auto mount_name = AllocateMountName();
        this->SetNames(mount_name, display_name);
        fsdevMountDevice(this->mnt_name.AsUTF8().c_str(), this->fs);
    }

    FspExplorer::FspExplorer(String display_name, std::string mount_name, FsFileSystem file_system) : StdExplorer(), dispose(false) {
        this->fs = file_system;
        this->SetNames(mount_name, display_name);
    }

    FspExplorer::~FspExplorer() {
        if(this->dispose) {
            fsdevUnmountDevice(this->mnt_name.AsUTF8().c_str());
            DisposeMountName(this->mnt_name.AsUTF8());
        }
    }

    bool FspExplorer::IsOk() {
        return serviceIsActive(&this->fs.s);
    }

    FsFileSystem *FspExplorer::GetFileSystem() {
        return &this->fs;
    }

    u64 FspExplorer::GetTotalSpace() {
        s64 size = 0;
        fsFsGetTotalSpace(&this->fs, "/", &size);
        return static_cast<u64>(size);
    }

    u64 FspExplorer::GetFreeSpace() {
        s64 size = 0;
        fsFsGetFreeSpace(&this->fs, "/", &size);
        return static_cast<u64>(size);
    }

    SdCardExplorer::SdCardExplorer() : FspExplorer("SdCard", "sdmc", *fsdevGetDeviceFileSystem("sdmc")) {}

    RomFsExplorer::RomFsExplorer() : FspExplorer("RomFs", "romfs", {}) {}

    NANDExplorer::NANDExplorer(Partition part) : FspExplorer(NANDExplorer::GetNANDPartitionName(part), NANDExplorer::MountNANDFileSystem(part)), part(part) {
        this->SetShouldWarnOnWriteAccess(true);
    }

    std::string NANDExplorer::GetNANDPartitionName(Partition part) {
        switch(part) {
            case Partition::PRODINFOF: {
                return "PRODINFOF";
            }
            case Partition::NANDSafe: {
                return "NAND-Safe";
            }
            case Partition::NANDUser: {
                return "NAND-User";
            }
            case Partition::NANDSystem: {
                return "NAND-System";
            }
            default: {
                return "NAND";
            }
        }
    }

    FsFileSystem NANDExplorer::MountNANDFileSystem(Partition part) {
        FsFileSystem fs;
        FsBisPartitionId id;
        switch(part) {
            case Partition::PRODINFOF: {
                id = FsBisPartitionId_CalibrationFile;
                break;
            }
            case Partition::NANDSafe: {
                id = FsBisPartitionId_SafeMode;
                break;
            }
            case Partition::NANDUser: {
                id = FsBisPartitionId_User;
                break;
            }
            case Partition::NANDSystem: {
                id = FsBisPartitionId_System;
                break;
            }
            default: {
                return fs;
            }
        }
        fsOpenBisFileSystem(&fs, id, "");
        return fs;
    }

    Partition NANDExplorer::GetPartition() {
        return this->part;
    }

    TitleSaveDataExplorer::TitleSaveDataExplorer(u64 app_id, AccountUid user_id) : FspExplorer(std::string("SaveData-0x") + hos::FormatApplicationId(app_id), TitleSaveDataExplorer::MountTitleSaveData(app_id, user_id)), appid(app_id), uid(user_id) {
        this->SetCommitFunction(std::bind(&TitleSaveDataExplorer::DoCommit, this));
    }

    FsFileSystem TitleSaveDataExplorer::MountTitleSaveData(u64 app_id, AccountUid user_id) {
        FsFileSystem fs;

        FsSaveDataAttribute attr = {};
        attr.application_id = app_id;
        attr.uid = user_id;
        attr.save_data_type = FsSaveDataType_Account;

        fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);
        return fs;
    }

    void TitleSaveDataExplorer::DoCommit() {
        fsdevCommitDevice(this->mnt_name.AsUTF8().c_str());
    }

    bool TitleSaveDataExplorer::Matches(u64 app_id, AccountUid user_id) {
        return (this->appid == app_id) && acc::UidCompare(&this->uid, &user_id);
    }

}