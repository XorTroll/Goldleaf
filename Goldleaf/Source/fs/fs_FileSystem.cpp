
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

namespace fs
{
    static SdCardExplorer *esdc = NULL;
    static NANDExplorer *eprd = NULL;
    static NANDExplorer *ensf = NULL;
    static NANDExplorer *enus = NULL;
    static NANDExplorer *enss = NULL;
    static RemotePCExplorer *epcdrv = NULL;

    SdCardExplorer *GetSdCardExplorer()
    {
        if(esdc == NULL) esdc = new SdCardExplorer();
        return esdc;
    }

    NANDExplorer *GetPRODINFOFExplorer()
    {
        if(eprd == NULL) eprd = new NANDExplorer(Partition::PRODINFOF);
        return eprd;
    }

    NANDExplorer *GetNANDSafeExplorer()
    {
        if(ensf == NULL) ensf = new NANDExplorer(Partition::NANDSafe);
        return ensf;
    }

    NANDExplorer *GetNANDUserExplorer()
    {
        if(enus == NULL) enus = new NANDExplorer(Partition::NANDUser);
        return enus;
    }

    NANDExplorer *GetNANDSystemExplorer()
    {
        if(enss == NULL) enss = new NANDExplorer(Partition::NANDSystem);
        return enss;
    }

    RemotePCExplorer *GetRemotePCExplorer(String MountName)
    {
        String mname = fs::GetPathRoot(MountName);
        if(epcdrv == NULL)
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

    Explorer *GetExplorerForMountName(String MountName)
    {
        Explorer *ex = NULL;
        if(esdc != NULL) if(esdc->GetMountName() == MountName) return esdc;
        if(eprd != NULL) if(eprd->GetMountName() == MountName) return eprd;
        if(ensf != NULL) if(ensf->GetMountName() == MountName) return ensf;
        if(enus != NULL) if(enus->GetMountName() == MountName) return enus;
        if(enss != NULL) if(enss->GetMountName() == MountName) return enss;
        if(epcdrv != NULL) if(epcdrv->GetMountName() == MountName) return epcdrv;
        return ex;
    }

    Explorer *GetExplorerForPath(String Path)
    {
        return GetExplorerForMountName(GetPathRoot(Path));
    }
}