
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

#include <nfp/nfp_Emuiibo.hpp>
#include <net/net_Network.hpp>
#include <fs/fs_FileSystem.hpp>

namespace nfp::emu
{
    static Service nfpemu_srv;
    static u64 nfpemu_refcnt = 0;

    bool IsEmuiiboAccessible()
    {
        Handle tmph = 0;
        auto rc = smRegisterService(&tmph, ServiceName, false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService(ServiceName);
        return false;
    }

    Result Initialize()
    {
        atomicIncrement64(&nfpemu_refcnt);
        if(serviceIsActive(&nfpemu_srv)) return 0;
        return smGetService(&nfpemu_srv, NFP_EMU_SERVICE);
    }

    void Exit()
    {
        if(atomicDecrement64(&nfpemu_refcnt) == 0) serviceClose(&nfpemu_srv);
    }

    Result GetCurrentAmiibo(char *out, size_t out_len)
    {
        return serviceDispatch(&nfpemu_srv, 0,
            .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
            .buffers = { { out, out_len } },
        );
    }

    Result SetCustomAmiibo(const char *path)
    {
        char inpath[FS_MAX_PATH] = {0};
        strcpy(inpath, path);

        return serviceDispatch(&nfpemu_srv, 1,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
            .buffers = { { inpath, FS_MAX_PATH } },
        );
    }

    Result HasCustomAmiibo(bool *out_has)
    {
        return serviceDispatchOut(&nfpemu_srv, 2, *out_has);
    }
    
    Result ResetCustomAmiibo()
    {
        return serviceDispatch(&nfpemu_srv, 3);
    }
    
    Result SetEmulationOnForever()
    {
        return serviceDispatch(&nfpemu_srv, 4);
    }
    
    Result SetEmulationOnOnce()
    {
        return serviceDispatch(&nfpemu_srv, 5);
    }
    
    Result SetEmulationOff()
    {
        return serviceDispatch(&nfpemu_srv, 6);
    }
    
    Result MoveToNextAmiibo()
    {
        return serviceDispatch(&nfpemu_srv, 7);
    }
    
    Result GetStatus(EmulationStatus *out)
    {
        return serviceDispatchOut(&nfpemu_srv, 8, *out);
    }
    
    Result Refresh()
    {
        return serviceDispatch(&nfpemu_srv, 9);
    }
    
    Result GetVersion(Version *out_ver)
    {
        return serviceDispatchOut(&nfpemu_srv, 10, *out_ver);
    }

    String SaveAmiiboImageById(String id)
    {
        std::string imgpath = (String("sdmc:/") + consts::Root + "/amiibocache/" + id + ".png").AsUTF8();
        if(fs::IsFile(imgpath)) return imgpath;
        auto json = net::RetrieveContent("https://www.amiiboapi.com/api/amiibo/?id=" + id.AsUTF8(), "application/json");
        if(!json.empty())
        {
            try
            {
                JSON j = JSON::parse(json);
                if(j.count("amiibo"))
                {
                    auto img = j["amiibo"].value("image", "");
                    if(!img.empty())
                    {
                        imgpath = (String("sdmc:/") + consts::Root + "/amiibocache/" + id + ".png").AsUTF8();
                        net::RetrieveToFile(img, imgpath, [&](double, double){});
                    }
                }
            }
            catch(std::exception&) {}
        }
        if(!fs::IsFile(imgpath)) return "";
        return imgpath;
    }

    VirtualAmiibo LoadVirtualAmiibo(String path)
    {
        VirtualAmiibo amiibo = {};
        std::ifstream ifs((path + "/model.json").AsUTF8().c_str());
        if(ifs.good())
        {
            try
            {
                JSON j = JSON::parse(ifs);
                amiibo.id = j.value("amiiboId", "");
            }
            catch(std::exception&) {}
            ifs.close();
            ifs.open((path + "/register.json").AsUTF8().c_str());
            if(ifs.good())
            {
                try
                {
                    JSON j = JSON::parse(ifs);
                    amiibo.name = j.value("name", "");
                }
                catch(std::exception&) {}
                ifs.close();
            }
        }

        return amiibo;
    }
}