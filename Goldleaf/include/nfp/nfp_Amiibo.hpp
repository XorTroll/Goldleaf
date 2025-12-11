
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <base.hpp>

namespace nfp {

    namespace json {

        struct Date {
            int d;
            int m;
            int y;
        };

        struct AmiiboId {
            int character_variant;
            int game_character_id;
            int series;
            int model_number;
            int figure_type;
        };

        using Uuid = std::vector<int>;

        struct Amiibo {
            Date first_write_date;
            Date last_write_date;
            std::string name;
            int version;
            int write_counter;
            AmiiboId id;
            Uuid uuid;
            std::string mii_charinfo_file;
        };

        struct AreaInfoEntry {
            u64 program_id;
            u32 access_id;
        };

        struct AreaInfo {
            u32 current_area_access_id;
            std::vector<AreaInfoEntry> areas;
        };

    }

    struct CharacterId {
        u16 game_character_id;
        u8 character_variant;
    } NX_PACKED;
    static_assert(sizeof(CharacterId) == 3);

    struct InternalAmiiboId {
        CharacterId character_id;
        u8 series;
        u16 model_number;
        u8 figure_type;
        u8 tag_type;
    };
    static_assert(sizeof(InternalAmiiboId) == 8);

    NX_CONSTEXPR InternalAmiiboId GetInternalAmiiboIdFromModelInfo(const NfpModelInfo &model_info) {
        InternalAmiiboId amiibo_id = {};
        amiibo_id.character_id.game_character_id = model_info.game_character_id;
        amiibo_id.character_id.character_variant = model_info.character_variant;
        amiibo_id.series = model_info.series_id;
        amiibo_id.model_number = model_info.numbering_id;
        amiibo_id.figure_type = model_info.nfp_type;
        amiibo_id.tag_type = 2;
        return amiibo_id;
    }

    struct AmiiboData {
        NfpTagInfo tag_info;
        NfpRegisterInfo register_info;
        NfpCommonInfo common_info;
        NfpModelInfo model_info;
        NfpAdminInfo admin_info;
        NfpData data;
    };

    Result Initialize();
    bool IsReady();
    Result Open();
    Result GetAmiiboData(AmiiboData &out_data, bool &out_has_register_info);
    std::string ExportAsVirtualAmiibo(const AmiiboData &data);
    void Close();
    void Exit();

}
