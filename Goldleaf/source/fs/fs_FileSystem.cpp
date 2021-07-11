
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

#include <fs/fs_FileSystem.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;

namespace fs {

    namespace {

        SdCardExplorer *g_SdCardExplorer = nullptr;
        RomFsExplorer *g_RomFsExplorer = nullptr;
        NANDExplorer *g_PRODINFOFExplorer = nullptr;
        NANDExplorer *g_NANDSafeExplorer = nullptr;
        NANDExplorer *g_NANDUserExplorer = nullptr;
        NANDExplorer *g_NANDSystemExplorer = nullptr;
        RemotePCExplorer *g_RemotePCExplorer = nullptr;
        DriveExplorer *g_DriveExplorer = nullptr;

    }

    SdCardExplorer *GetSdCardExplorer() {
        if(g_SdCardExplorer == nullptr) {
            g_SdCardExplorer = new SdCardExplorer();
        }
        return g_SdCardExplorer;
    }

    RomFsExplorer *GetRomFsExplorer() {
        if(g_RomFsExplorer == nullptr) {
            g_RomFsExplorer = new RomFsExplorer();
        }
        return g_RomFsExplorer;
    }

    NANDExplorer *GetPRODINFOFExplorer() {
        if(g_PRODINFOFExplorer == nullptr) {
            g_PRODINFOFExplorer = new NANDExplorer(Partition::PRODINFOF);
        }
        return g_PRODINFOFExplorer;
    }

    NANDExplorer *GetNANDSafeExplorer() {
        if(g_NANDSafeExplorer == nullptr) {
            g_NANDSafeExplorer = new NANDExplorer(Partition::NANDSafe);
        }
        return g_NANDSafeExplorer;
    }

    NANDExplorer *GetNANDUserExplorer() {
        if(g_NANDUserExplorer == nullptr) {
            g_NANDUserExplorer = new NANDExplorer(Partition::NANDUser);
        }
        return g_NANDUserExplorer;
    }

    NANDExplorer *GetNANDSystemExplorer() {
        if(g_NANDSystemExplorer == nullptr) {
            g_NANDSystemExplorer = new NANDExplorer(Partition::NANDSystem);
        }
        return g_NANDSystemExplorer;
    }

    RemotePCExplorer *GetRemotePCExplorer(String mount_name) {
        const auto mnt_name_root = fs::GetPathRoot(mount_name);
        if(g_RemotePCExplorer == nullptr) {
            g_RemotePCExplorer = new RemotePCExplorer(mnt_name_root);
            if(mount_name != mnt_name_root) {
                String pth = fs::GetPathWithoutRoot(mount_name);
                pth.erase(0, 1);
                g_RemotePCExplorer->NavigateForward(pth);
            }
        }
        else {
            if(g_RemotePCExplorer->GetMountName() != mount_name) {
                delete g_RemotePCExplorer;
                g_RemotePCExplorer = new RemotePCExplorer(mnt_name_root);
                if(mount_name != mnt_name_root) {
                    String pth = fs::GetPathWithoutRoot(mount_name);
                    pth.erase(0, 1);
                    g_RemotePCExplorer->NavigateForward(pth);
                }
            }
        }
        return g_RemotePCExplorer;
    }

    DriveExplorer *GetDriveExplorer(UsbHsFsDevice &drive) {
        if(g_DriveExplorer == nullptr) {
            g_DriveExplorer = new DriveExplorer(drive);
        }
        else {
            if(!drive::DrivesEqual(g_DriveExplorer->GetDrive(), drive)) {
                delete g_DriveExplorer;
                g_DriveExplorer = new DriveExplorer(drive);
            }
        }
        return g_DriveExplorer;
    }

    Explorer *GetExplorerForMountName(String mount_name) {
        if((g_SdCardExplorer != nullptr) && (g_SdCardExplorer->GetMountName() == mount_name)) {
            return g_SdCardExplorer;
        }

        if((g_RomFsExplorer != nullptr) && (g_RomFsExplorer->GetMountName() == mount_name)) {
            return g_RomFsExplorer;
        }

        if((g_PRODINFOFExplorer != nullptr) && (g_PRODINFOFExplorer->GetMountName() == mount_name)) {
            return g_PRODINFOFExplorer;
        }
        
        if((g_NANDSafeExplorer != nullptr) && (g_NANDSafeExplorer->GetMountName() == mount_name)) {
            return g_NANDSafeExplorer;
        }

        if((g_NANDUserExplorer != nullptr) && (g_NANDUserExplorer->GetMountName() == mount_name)) {
            return g_NANDUserExplorer;
        }

        if((g_NANDSystemExplorer != nullptr) && (g_NANDSystemExplorer->GetMountName() == mount_name)) {
            return g_NANDSystemExplorer;
        }

        if((g_RemotePCExplorer != nullptr) && (g_RemotePCExplorer->GetMountName() == mount_name)) {
            return g_RemotePCExplorer;
        }

        if((g_DriveExplorer != nullptr) && (g_DriveExplorer->GetMountName() == mount_name)) {
            return g_DriveExplorer;
        }

        auto &mounted_exps = g_MainApplication->GetExploreMenuLayout()->GetMountedExplorers();
        for(auto &exp: mounted_exps) {
            if(exp->GetMountName() == mount_name) {
                return exp;
            }
        }
        return nullptr;
    }

    Explorer *GetExplorerForPath(String path) {
        return GetExplorerForMountName(GetPathRoot(path));
    }

}