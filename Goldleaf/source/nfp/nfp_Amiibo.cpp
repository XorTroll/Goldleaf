
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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
#include <hos/hos_Common.hpp>

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
                if(R_SUCCEEDED(rc)) {
                    g_Initialized = true;
                }
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

    NfpData GetAll() {
        NfpData data = {};
        serviceDispatchIn(nfpGetServiceSession_Interface(), 200, g_DeviceHandle,
            .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
            .buffers = { { &data, sizeof(data) } },
        );
        return data;
    }

    std::string ExportAsVirtualAmiibo(const NfpTagInfo &tag, const NfpRegisterInfo &reg, const NfpCommonInfo &common, const NfpModelInfo &model, const NfpData &all) {
        auto sd_exp = fs::GetSdCardExplorer();

        std::string emuiibo_path = "emuiibo";
        sd_exp->CreateDirectory(emuiibo_path);
        emuiibo_path += "/amiibo";
        sd_exp->CreateDirectory(emuiibo_path);

        std::string amiibo_folder = reg.amiibo_name;
        auto amiibo_path = emuiibo_path + "/" + amiibo_folder;
        auto i = 1;
        while(sd_exp->IsDirectory(amiibo_path)) {
            amiibo_folder = std::string(reg.amiibo_name) + "_" + std::to_string(i);
            amiibo_path = emuiibo_path + "/" + amiibo_folder;
            i++;
        }
        sd_exp->CreateDirectory(amiibo_path);
        sd_exp->CreateFile(amiibo_path + "/amiibo.flag");

        auto amiibo = JSON::object();
        amiibo["first_write_date"]["d"] = reg.first_write_day;
        amiibo["first_write_date"]["m"] = reg.first_write_month;
        amiibo["first_write_date"]["y"] = reg.first_write_year;

        amiibo["last_write_date"]["d"] = common.last_write_day;
        amiibo["last_write_date"]["m"] = common.last_write_month;
        amiibo["last_write_date"]["y"] = common.last_write_year;

        const auto mii_charinfo = "mii-charinfo.bin";
        amiibo["mii_charinfo_file"] = mii_charinfo;
        sd_exp->WriteFile(amiibo_path + "/" + mii_charinfo, &reg.mii, sizeof(reg.mii));

        const auto legacy_mii = "legacy-mii.bin";
        sd_exp->WriteFile(amiibo_path + "/" + legacy_mii, all.legacy_mii, sizeof(all.legacy_mii));

        for(u32 i = 0; i < sizeof(tag.uuid); i++) {
            amiibo["uuid"][i] = tag.uuid[i];
        }

        amiibo["name"] = reg.amiibo_name;

        amiibo["version"] = common.version;

        amiibo["write_counter"] = common.write_counter;

        const auto id_ref = reinterpret_cast<const AmiiboId*>(model.amiibo_id);
        amiibo["id"]["character_variant"] = id_ref->character_id.character_variant;
        amiibo["id"]["game_character_id"] = id_ref->character_id.game_character_id;
        amiibo["id"]["series"] = id_ref->series;
        amiibo["id"]["model_number"] = __builtin_bswap16(id_ref->model_number);
        amiibo["id"]["figure_type"] = id_ref->figure_type;

        sd_exp->WriteJSON(amiibo_path + "/amiibo.json", amiibo);

        // If the amiibo has application area...
        if(all.admin_info.flags & BIT(1)) {
            auto area_info = JSON::object();
            area_info["current_area_access_id"] = all.admin_info.access_id;
            area_info["areas"][0]["program_id"] = all.admin_info.program_id;
            area_info["areas"][0]["access_id"] = all.admin_info.access_id;

            sd_exp->WriteJSON(amiibo_path + "/areas.json", area_info);

            const auto areas_dir = amiibo_path + "/areas";
            sd_exp->CreateDirectory(areas_dir);

            const auto area_path = areas_dir + "/" + hos::FormatHex(all.admin_info.access_id) + ".bin";
            sd_exp->WriteFile(area_path, all.app_area, sizeof(all.app_area));
        }

        return amiibo_folder;
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