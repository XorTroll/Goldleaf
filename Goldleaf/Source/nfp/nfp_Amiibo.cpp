
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

#include <nfp/nfp_Amiibo.hpp>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

namespace nfp
{
    static NfcDeviceHandle dhandle;
    static Event available;
    static Event activate;
    static Event deactivate;
    static bool init;

    Result Initialize()
    {
        if(init) return 0;
        Result rc = nfpInitialize(NfpServiceType_Debug);
        if(R_SUCCEEDED(rc))
        {
            rc = nfpAttachAvailabilityChangeEvent(&available);
            if(R_SUCCEEDED(rc))
            {
                rc = nfpListDevices(NULL, &dhandle, 1);
                if(R_SUCCEEDED(rc))
                {
                    rc = nfpAttachActivateEvent(&dhandle, &activate);
                    if(R_SUCCEEDED(rc))
                    {
                        rc = nfpAttachDeactivateEvent(&dhandle, &deactivate);
                        if(R_SUCCEEDED(rc))
                        {
                            rc = nfpStartDetection(&dhandle);
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
        NfpDeviceState dst;
        nfpGetDeviceState(&dhandle, &dst);
        return (dst == NfpDeviceState_TagFound);
    }

    Result Open()
    {
        return nfpMount(&dhandle, NfpDeviceType_Amiibo, NfpMountTarget_All);
    }

    NfpTagInfo GetTagInfo()
    {
        NfpTagInfo tinfo = {0};
        nfpGetTagInfo(&dhandle, &tinfo);
        return tinfo;
    }

    NfpRegisterInfo GetRegisterInfo()
    {
        NfpRegisterInfo rinfo = {0};
        nfpGetRegisterInfo(&dhandle, &rinfo);
        return rinfo;
    }

    NfpCommonInfo GetCommonInfo()
    {
        NfpCommonInfo cinfo = {0};
        nfpGetCommonInfo(&dhandle, &cinfo);
        return cinfo;
    }

    NfpModelInfo GetModelInfo()
    {
        NfpModelInfo minfo = {0};
        nfpGetModelInfo(&dhandle, &minfo);
        return minfo;
    }

    Result DumpToEmuiibo(NfpTagInfo &tag, NfpRegisterInfo &reg, NfpCommonInfo &common, NfpModelInfo &model)
    {
        auto outdir = "sdmc:/emuiibo/amiibo/" + String(reg.amiibo_name);
        fsdevDeleteDirectoryRecursively(outdir.AsUTF8().c_str());

        mkdir("sdmc:/emuiibo", 777);
        mkdir("sdmc:/emuiibo/amiibo", 777);
        mkdir(outdir.AsUTF8().c_str(), 777);

        auto jtag = JSON::object();
        std::stringstream strm;
        for(u32 i = 0; i < 9; i++) strm << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << (int)tag.uuid[i];
        jtag["uuid"] = strm.str();
        std::ofstream ofs((outdir + "/tag.json").AsUTF8());
        ofs << std::setw(4) << jtag;
        ofs.close();

        auto jmodel = JSON::object();
        strm.str("");
        strm.clear();
        for(u32 i = 0; i < 8; i++) strm << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << (int)model.amiibo_id[i];
        jmodel["amiiboId"] = strm.str();
        ofs = std::ofstream((outdir + "/model.json").AsUTF8());
        ofs << std::setw(4) << jmodel;
        ofs.close();

        FILE *f = fopen((outdir + "/mii-charinfo.bin").AsUTF8().c_str(), "wb");
        fwrite(&reg.mii, 1, sizeof(NfpMiiCharInfo), f);
        fclose(f);

        auto jreg = JSON::object();
        jreg["name"] = std::string(reg.amiibo_name);
        jreg["miiCharInfo"] = "mii-charinfo.bin";
        strm.str("");
        strm.clear();
        strm << std::dec << reg.first_write_year << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)reg.first_write_month << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)reg.first_write_day;
        jreg["firstWriteDate"] = strm.str();
        ofs = std::ofstream((outdir + "/register.json").AsUTF8());
        ofs << std::setw(4) << jreg;
        ofs.close();

        auto jcommon = JSON::object();
        strm.str("");
        strm.clear();
        strm << std::dec << common.last_write_year << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)common.last_write_month << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)common.last_write_day;
        jcommon["lastWriteDate"] = strm.str();
        jcommon["writeCounter"] = (int)common.write_counter;
        jcommon["version"] = (int)common.version;
        ofs = std::ofstream((outdir + "/common.json").AsUTF8());
        ofs << std::setw(4) << jcommon;
        ofs.close();

        return 0;
    }

    void Close()
    {
        nfpUnmount(&dhandle);
    }

    void Exit()
    {
        if(!init) return;
        nfpStopDetection(&dhandle);
        nfpExit();
        init = false;
    }
}