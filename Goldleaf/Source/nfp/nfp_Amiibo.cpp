#include <nfp/nfp_Amiibo.hpp>

namespace nfp
{
    static HidControllerID dhandle;
    static Event available;
    static Event activate;
    static Event deactivate;
    static bool init;

    Result Initialize()
    {
        if(init) return 0;
        Result rc = nfpuInitialize(NULL);
        if(rc == 0)
        {
            rc = nfpuAttachAvailabilityChangeEvent(&available);
            if(rc == 0)
            {
                HidControllerID devs[9];
                rc = nfpuListDevices(NULL, devs, 9);
                if(rc == 0)
                {
                    dhandle = devs[0];
                    rc = nfpuAttachActivateEvent(dhandle, &activate);
                    if(rc == 0)
                    {
                        rc = nfpuAttachDeactivateEvent(dhandle, &deactivate);
                        if(rc == 0)
                        {
                            rc = nfpuStartDetection(dhandle);
                            init = true;
                        }
                    }
                }
            }
        }
        return rc;
    }

    bool IsReady()
    {
        if(!init) return false;
        NfpuDeviceState dst;
        nfpuGetDeviceState(dhandle, &dst);
        return (dst == NfpuDeviceState_TagFound);
    }

    Result Open()
    {
        return nfpuMount(dhandle, NfpuDeviceType_Amiibo, NfpuMountTarget_All);
    }

    NfpuTagInfo GetTagInfo()
    {
        NfpuTagInfo tinfo = {0};
        nfpuGetTagInfo(dhandle, &tinfo);
        return tinfo;
    }

    NfpuRegisterInfo GetRegisterInfo()
    {
        NfpuRegisterInfo rinfo = {0};
        nfpuGetRegisterInfo(dhandle, &rinfo);
        return rinfo;
    }

    NfpuCommonInfo GetCommonInfo()
    {
        NfpuCommonInfo cinfo = {0};
        nfpuGetCommonInfo(dhandle, &cinfo);
        return cinfo;
    }

    NfpuModelInfo GetModelInfo()
    {
        NfpuModelInfo minfo = {0};
        nfpuGetModelInfo(dhandle, &minfo);
        return minfo;
    }

    Result DumpToEmuiibo()
    {
        auto tinfo = GetTagInfo();
        auto rinfo = GetRegisterInfo();
        auto minfo = GetModelInfo();
        auto cinfo = GetCommonInfo();
        JSON jdata;
        jdata["name"] = rinfo.amiibo_name;
        jdata["applicationAreaSize"] = (int)cinfo.application_area_size;
        jdata["firstWriteDate"] = { (int)rinfo.first_write_year, (int)rinfo.first_write_month, (int)rinfo.first_write_day };
        jdata["lastWriteDate"] = { (int)cinfo.last_write_year, (int)cinfo.last_write_month, (int)cinfo.last_write_day };
        fs::CreateDirectory(("sdmc:/emuiibo/" + std::string(rinfo.amiibo_name)).c_str());
        std::ofstream ofs("sdmc:/emuiibo/" + std::string(rinfo.amiibo_name) + "/amiibo.json");
        ofs << jdata.dump(4);
        FILE *f = fopen(("sdmc:/emuiibo/" + std::string(rinfo.amiibo_name) + "/mii.dat").c_str(), "wb");
        fwrite(&rinfo.mii, 1, sizeof(NfpuMiiCharInfo), f);
        fclose(f);
        AmiiboData bin = {0};
        memcpy(bin.uuid, tinfo.uuid, tinfo.uuid_length);
        memcpy(bin.amiibo_id, minfo.amiibo_id, 8);
        f = fopen(("sdmc:/emuiibo/" + std::string(rinfo.amiibo_name) + "/amiibo.bin").c_str(), "wb");
        fwrite(&bin, 1, sizeof(AmiiboData), f);
        fclose(f);
        return 0;
    }

    void Close()
    {
        nfpuUnmount(dhandle);
    }

    void Finalize()
    {
        if(!init) return;
        nfpuStopDetection(dhandle);
        nfpuExit();
        init = false;
    }
}