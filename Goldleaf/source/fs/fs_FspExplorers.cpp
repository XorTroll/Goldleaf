
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

        void DisposeMountName(const std::string &name) {
            g_MountNameTable.erase(std::remove(g_MountNameTable.begin(), g_MountNameTable.end(), name), g_MountNameTable.end());
        }

    }

    FspExplorer::FspExplorer(FsFileSystem fs, const std::string &display_name, const std::string &mount_name) : StdExplorer(), fs(fs), dispose(mount_name.empty()) {
        auto fs_mount_name = mount_name;
        const auto needs_to_handle_mount = mount_name.empty();
        if(needs_to_handle_mount) {
            fs_mount_name = AllocateMountName();
        }

        this->SetNames(fs_mount_name, display_name);

        if(needs_to_handle_mount) {
            fsdevMountDevice(this->mnt_name.c_str(), this->fs);
        }
    }

    FspExplorer::~FspExplorer() {
        if(this->dispose) {
            fsdevUnmountDevice(this->mnt_name.c_str());
            DisposeMountName(this->mnt_name);
        }
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

    NANDExplorer::NANDExplorer(const Partition part) : FspExplorer(NANDExplorer::MountNANDFileSystem(part), NANDExplorer::GetNANDPartitionName(part)), part(part) {
        this->SetShouldWarnOnWriteAccess(true);
    }

    std::string NANDExplorer::GetNANDPartitionName(const Partition part) {
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

    FsFileSystem NANDExplorer::MountNANDFileSystem(const Partition part) {
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

        GLEAF_RC_ASSERT(fsOpenBisFileSystem(&fs, id, ""));
        return fs;
    }

}