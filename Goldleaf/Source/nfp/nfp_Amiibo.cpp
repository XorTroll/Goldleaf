
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

namespace nfp
{
    static NfcDeviceHandle dhandle;
    static bool init = false;

    Result Initialize()
    {
        if(init) return 0;
        auto rc = nfpInitialize(NfpServiceType_Debug);
        if(R_SUCCEEDED(rc))
        {
            rc = nfpListDevices(nullptr, &dhandle, 1);
            if(R_SUCCEEDED(rc))
            {
                rc = nfpStartDetection(&dhandle);
                init = true;
            }
        }
        return rc;
    }

    bool IsReady()
    {
        if(!init) return false;
        NfpDeviceState dst = NfpDeviceState_Unavailable;
        nfpGetDeviceState(&dhandle, &dst);
        return dst == NfpDeviceState_TagFound;
    }

    Result Open()
    {
        return nfpMount(&dhandle, NfpDeviceType_Amiibo, NfpMountTarget_All);
    }

    NfpTagInfo GetTagInfo()
    {
        NfpTagInfo tinfo = {};
        nfpGetTagInfo(&dhandle, &tinfo);
        return tinfo;
    }

    NfpRegisterInfo GetRegisterInfo()
    {
        NfpRegisterInfo rinfo = {};
        nfpGetRegisterInfo(&dhandle, &rinfo);
        return rinfo;
    }

    NfpCommonInfo GetCommonInfo()
    {
        NfpCommonInfo cinfo = {};
        nfpGetCommonInfo(&dhandle, &cinfo);
        return cinfo;
    }

    NfpModelInfo GetModelInfo()
    {
        NfpModelInfo minfo = {};
        nfpGetModelInfo(&dhandle, &minfo);

        minfo.amiibo_id[5] = minfo.amiibo_id[4];
        minfo.amiibo_id[4] = 0;
        minfo.amiibo_id[7] = 2;
        
        return minfo;
    }

    Result DumpToEmuiibo(NfpTagInfo &tag, NfpRegisterInfo &reg, NfpCommonInfo &common, NfpModelInfo &model)
    {
        auto sd_exp = fs::GetSdCardExplorer();

        String emuiibo_path = "emuiibo";
        sd_exp->CreateDirectory(emuiibo_path);
        emuiibo_path += "/amiibo";
        sd_exp->CreateDirectory(emuiibo_path);

        auto amiibo_path = emuiibo_path + "/" + reg.amiibo_name;
        sd_exp->DeleteDirectory(amiibo_path);
        sd_exp->CreateDirectory(amiibo_path);
        
        sd_exp->CreateFile(amiibo_path + "/amiibo.flag");

        auto amiibo = JSON::object();
        
        amiibo["first_write_date"]["d"] = static_cast<u8>(reg.first_write_day);
        amiibo["first_write_date"]["m"] = static_cast<u8>(reg.first_write_month);
        amiibo["first_write_date"]["y"] = static_cast<u16>(reg.first_write_year);

        amiibo["last_write_date"]["d"] = static_cast<u8>(common.last_write_day);
        amiibo["last_write_date"]["m"] = static_cast<u8>(common.last_write_month);
        amiibo["last_write_date"]["y"] = static_cast<u16>(common.last_write_year);

        auto mii_charinfo = "mii-charinfo.bin";
        amiibo["mii_charinfo_file"] = mii_charinfo;
        sd_exp->WriteFileBlock(amiibo_path + "/" + mii_charinfo, &reg.mii, sizeof(reg.mii));

        for(u32 i = 0; i < sizeof(tag.uuid); i++) amiibo["uuid"][i] = tag.uuid[i];

        amiibo["name"] = reg.amiibo_name;

        amiibo["version"] = static_cast<u16>(common.version);

        amiibo["write_counter"] = static_cast<u16>(common.write_counter);

        auto id_ref = reinterpret_cast<AmiiboId*>(model.amiibo_id);
        amiibo["id"]["character_variant"] = id_ref->character_id.character_variant;
        amiibo["id"]["game_character_id"] = static_cast<u16>(id_ref->character_id.game_character_id);
        amiibo["id"]["series"] = id_ref->series;
        amiibo["id"]["model_number"] = __builtin_bswap16(static_cast<u16>(id_ref->model_number));
        amiibo["id"]["figure_type"] = id_ref->figure_type;
        
        auto amiibo_json = amiibo.dump(4);
        sd_exp->WriteFileBlock(amiibo_path + "/amiibo.json", amiibo_json.data(), amiibo_json.length());
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