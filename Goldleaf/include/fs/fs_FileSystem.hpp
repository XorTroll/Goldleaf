
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

#pragma once
#include <fs/fs_Common.hpp>
#include <fs/fs_Explorer.hpp>
#include <fs/fs_StdExplorer.hpp>
#include <fs/fs_FspExplorers.hpp>
#include <fs/fs_DriveExplorer.hpp>
#include <fs/fs_RemotePCExplorer.hpp>

namespace fs {

    SdCardExplorer *GetSdCardExplorer();
    RomFsExplorer *GetRomFsExplorer(); 
    NANDExplorer *GetPRODINFOFExplorer();
    NANDExplorer *GetNANDSafeExplorer();
    NANDExplorer *GetNANDUserExplorer();
    NANDExplorer *GetNANDSystemExplorer();
    RemotePCExplorer *GetRemotePCExplorer(String mount_name);
    DriveExplorer *GetDriveExplorer(UsbHsFsDevice &drive);
    Explorer *GetExplorerForMountName(String mount_name);
    Explorer *GetExplorerForPath(String path);

    inline void Initialize() {
        GetSdCardExplorer();
        GetRomFsExplorer(); 
        GetPRODINFOFExplorer();
        GetNANDSafeExplorer();
        GetNANDUserExplorer();
        GetNANDSystemExplorer();
    }

    void Finalize();

}