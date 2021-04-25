
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

#include <nfp/nfp_Amiibo.hpp>
#include <fs/fs_FileSystem.hpp>

namespace nfp {

    namespace {

        NfcDeviceHandle g_DeviceHandle;
        bool g_Initialized = false;

    }

    Result Initialize() {
        if(g_Initialized) {
            return 0;
        }
        auto rc = nfpInitialize(NfpServiceType_Debug);
        if(R_SUCCEEDED(rc)) {
            rc = nfpListDevices(nullptr, &g_DeviceHandle, 1);
            if(R_SUCCEEDED(rc)) {
                rc = nfpStartDetection(&g_DeviceHandle);
                g_Initialized = true;
            }
        }
        return rc;
    }

    bool IsReady() {
        if(!g_Initialized) {
            return false;
        }

        auto dev_state = NfpDeviceState_Unavailable;
        nfpGetDeviceState(&g_DeviceHandle, &dev_state);
        return dev_state == NfpDeviceState_TagFound;
    }

    Result Open() {
        return nfpMount(&g_DeviceHandle, NfpDeviceType_Amiibo, NfpMountTarget_All);
    }

    NfpTagInfo GetTagInfo() {
        NfpTagInfo tag_info = {};
        nfpGetTagInfo(&g_DeviceHandle, &tag_info);
        return tag_info;
    }

    NfpRegisterInfo GetRegisterInfo() {
        NfpRegisterInfo reg_info = {};
        nfpGetRegisterInfo(&g_DeviceHandle, &reg_info);
        return reg_info;
    }

    NfpCommonInfo GetCommonInfo() {
        NfpCommonInfo common_info = {};
        nfpGetCommonInfo(&g_DeviceHandle, &common_info);
        return common_info;
    }

    NfpModelInfo GetModelInfo() {
        NfpModelInfo model_info = {};
        nfpGetModelInfo(&g_DeviceHandle, &model_info);

        model_info.amiibo_id[5] = model_info.amiibo_id[4];
        model_info.amiibo_id[4] = 0;
        model_info.amiibo_id[7] = 2;
        return model_info;
    }

    void DumpToEmuiibo(const NfpTagInfo &tag, const NfpRegisterInfo &reg, const NfpCommonInfo &common, const NfpModelInfo &model) {
        auto sd_exp = fs::GetSdCardExplorer();

        String emuiibo_path = "emuiibo";
        sd_exp->CreateDirectory(emuiibo_path);
        emuiibo_path += "/amiibo";
        sd_exp->CreateDirectory(emuiibo_path);

        const auto amiibo_path = emuiibo_path + "/" + reg.amiibo_name;
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

        const auto mii_charinfo = "mii-charinfo.bin";
        amiibo["mii_charinfo_file"] = mii_charinfo;
        sd_exp->WriteFile(amiibo_path + "/" + mii_charinfo, &reg.mii, sizeof(reg.mii));

        for(u32 i = 0; i < sizeof(tag.uuid); i++) {
            amiibo["uuid"][i] = tag.uuid[i];
        }

        amiibo["name"] = reg.amiibo_name;

        amiibo["version"] = static_cast<u16>(common.version);

        amiibo["write_counter"] = static_cast<u16>(common.write_counter);

        const auto id_ref = reinterpret_cast<const AmiiboId*>(model.amiibo_id);
        amiibo["id"]["character_variant"] = id_ref->character_id.character_variant;
        amiibo["id"]["game_character_id"] = static_cast<u16>(id_ref->character_id.game_character_id);
        amiibo["id"]["series"] = id_ref->series;
        amiibo["id"]["model_number"] = __builtin_bswap16(static_cast<u16>(id_ref->model_number));
        amiibo["id"]["figure_type"] = id_ref->figure_type;

        const auto amiibo_json = amiibo.dump(4);
        sd_exp->WriteFile(amiibo_path + "/amiibo.json", amiibo_json.c_str(), amiibo_json.length());
    }

    void Close() {
        nfpUnmount(&g_DeviceHandle);
    }

    void Exit() {
        if(g_Initialized) {
            nfpStopDetection(&g_DeviceHandle);
            nfpExit();
            g_Initialized = false;
        }
    }

}