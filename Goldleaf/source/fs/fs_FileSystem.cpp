
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

        template<typename T, typename ...Args>
        inline T *EnsureExplorer(T *&exp, Args &&...args) {
            if(exp == nullptr) {
                exp = new T(args...);
            }
            return exp;
        }

        template<typename T>
        inline void DeleteExplorer(T *&exp) {
            if(exp != nullptr) {
                delete exp;
                exp = nullptr;
            }
        }

        template<typename T>
        inline bool IsExplorer(T *&exp, String mount_name) {
            return (exp != nullptr) && (exp->GetMountName() == mount_name);
        }

    }

    SdCardExplorer *GetSdCardExplorer() {
        return EnsureExplorer(g_SdCardExplorer);
    }

    RomFsExplorer *GetRomFsExplorer() {
        return EnsureExplorer(g_RomFsExplorer);
    }

    NANDExplorer *GetPRODINFOFExplorer() {
        return EnsureExplorer(g_PRODINFOFExplorer, Partition::PRODINFOF);
    }

    NANDExplorer *GetNANDSafeExplorer() {
        return EnsureExplorer(g_NANDSafeExplorer, Partition::NANDSafe);
    }

    NANDExplorer *GetNANDUserExplorer() {
        return EnsureExplorer(g_NANDUserExplorer, Partition::NANDUser);
    }

    NANDExplorer *GetNANDSystemExplorer() {
        return EnsureExplorer(g_NANDSystemExplorer, Partition::NANDSystem);
    }

    RemotePCExplorer *GetRemotePCExplorer(String mount_name) {
        if((g_RemotePCExplorer != nullptr) && (g_RemotePCExplorer->GetMountName() != mount_name)) {
            DeleteExplorer(g_RemotePCExplorer);
        }

        const auto mnt_name_root = fs::GetPathRoot(mount_name);
        EnsureExplorer(g_RemotePCExplorer, mnt_name_root);
        
        if(mount_name != mnt_name_root) {
            auto base_path = fs::GetPathWithoutRoot(mount_name);
            base_path.erase(0, 1);
            g_RemotePCExplorer->NavigateForward(base_path);
        }
        return g_RemotePCExplorer;
    }

    DriveExplorer *GetDriveExplorer(UsbHsFsDevice &drive) {
        if((g_DriveExplorer != nullptr) && !drive::DrivesEqual(g_DriveExplorer->GetDrive(), drive)) {
            DeleteExplorer(g_DriveExplorer);
        }

        return EnsureExplorer(g_DriveExplorer, drive);
    }

    Explorer *GetExplorerForMountName(String mount_name) {
        if(IsExplorer(g_SdCardExplorer, mount_name)) {
            return g_SdCardExplorer;
        }
        if(IsExplorer(g_RomFsExplorer, mount_name)) {
            return g_RomFsExplorer;
        }
        if(IsExplorer(g_PRODINFOFExplorer, mount_name)) {
            return g_PRODINFOFExplorer;
        }
        if(IsExplorer(g_NANDSafeExplorer, mount_name)) {
            return g_NANDSafeExplorer;
        }
        if(IsExplorer(g_NANDUserExplorer, mount_name)) {
            return g_NANDUserExplorer;
        }
        if(IsExplorer(g_NANDSystemExplorer, mount_name)) {
            return g_NANDSystemExplorer;
        }
        if(IsExplorer(g_RemotePCExplorer, mount_name)) {
            return g_RemotePCExplorer;
        }
        if(IsExplorer(g_DriveExplorer, mount_name)) {
            return g_DriveExplorer;
        }

        auto &mounted_exps = g_MainApplication->GetExploreMenuLayout()->GetMountedExplorers();
        for(auto &exp: mounted_exps) {
            if(IsExplorer(exp, mount_name)) {
                return exp;
            }
        }
        return nullptr;
    }

    Explorer *GetExplorerForPath(String path) {
        return GetExplorerForMountName(GetPathRoot(path));
    }

    void Finalize() {
        DeleteExplorer(g_SdCardExplorer);
        DeleteExplorer(g_RomFsExplorer);
        DeleteExplorer(g_PRODINFOFExplorer);
        DeleteExplorer(g_NANDSafeExplorer);
        DeleteExplorer(g_NANDUserExplorer);
        DeleteExplorer(g_NANDSystemExplorer);
        DeleteExplorer(g_RemotePCExplorer);
        DeleteExplorer(g_DriveExplorer);

        auto work_buf = GetWorkBuffer();
        operator delete[](work_buf, std::align_val_t(0x1000));
    }

}