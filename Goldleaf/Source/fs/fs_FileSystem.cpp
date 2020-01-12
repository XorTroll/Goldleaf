
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
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;

namespace fs
{
    static SdCardExplorer *esdc = nullptr;
    static NANDExplorer *eprd = nullptr;
    static NANDExplorer *ensf = nullptr;
    static NANDExplorer *enus = nullptr;
    static NANDExplorer *enss = nullptr;
    static RemotePCExplorer *epcdrv = nullptr;
    static USBDriveExplorer *eusbdrv = nullptr;

    SdCardExplorer *GetSdCardExplorer()
    {
        if(esdc == nullptr) esdc = new SdCardExplorer();
        return esdc;
    }

    NANDExplorer *GetPRODINFOFExplorer()
    {
        if(eprd == nullptr) eprd = new NANDExplorer(Partition::PRODINFOF);
        return eprd;
    }

    NANDExplorer *GetNANDSafeExplorer()
    {
        if(ensf == nullptr) ensf = new NANDExplorer(Partition::NANDSafe);
        return ensf;
    }

    NANDExplorer *GetNANDUserExplorer()
    {
        if(enus == nullptr) enus = new NANDExplorer(Partition::NANDUser);
        return enus;
    }

    NANDExplorer *GetNANDSystemExplorer()
    {
        if(enss == nullptr) enss = new NANDExplorer(Partition::NANDSystem);
        return enss;
    }

    RemotePCExplorer *GetRemotePCExplorer(String MountName)
    {
        String mname = fs::GetPathRoot(MountName);
        if(epcdrv == nullptr)
        {
            epcdrv = new RemotePCExplorer(mname);
            if(MountName != mname)
            {
                String pth = fs::GetPathWithoutRoot(MountName);
                pth.erase(0, 1);
                epcdrv->NavigateForward(pth);
            }
        }
        else
        {
            if(epcdrv->GetMountName() != MountName)
            {
                delete epcdrv;
                epcdrv = new RemotePCExplorer(mname);
                if(MountName != mname)
                {
                    String pth = fs::GetPathWithoutRoot(MountName);
                    pth.erase(0, 1);
                    epcdrv->NavigateForward(pth);
                }
            }
        }
        return epcdrv;
    }

    USBDriveExplorer *GetUSBDriveExplorer(drive::Drive drive)
    {
        if(eusbdrv == nullptr) eusbdrv = new USBDriveExplorer(drive);
        else
        {
            auto drv = eusbdrv->GetDrive();
            if(drv.interface_id != drive.interface_id)
            {
                delete eusbdrv;
                eusbdrv = new USBDriveExplorer(drive);
            }
        }
        return eusbdrv;
    }

    Explorer *GetExplorerForMountName(String MountName)
    {
        if(esdc != nullptr) if(esdc->GetMountName() == MountName) return esdc;
        if(eprd != nullptr) if(eprd->GetMountName() == MountName) return eprd;
        if(ensf != nullptr) if(ensf->GetMountName() == MountName) return ensf;
        if(enus != nullptr) if(enus->GetMountName() == MountName) return enus;
        if(enss != nullptr) if(enss->GetMountName() == MountName) return enss;
        if(epcdrv != nullptr) if(epcdrv->GetMountName() == MountName) return epcdrv;
        if(eusbdrv != nullptr) if(eusbdrv->GetMountName() == MountName) return eusbdrv;
        auto &mounted_exps = global_app->GetExploreMenuLayout()->GetMountedExplorers();
        for(auto exp: mounted_exps)
        {
            if(exp->GetMountName() == MountName) return exp;
        }
        return nullptr;
    }

    Explorer *GetExplorerForPath(String Path)
    {
        return GetExplorerForMountName(GetPathRoot(Path));
    }
}