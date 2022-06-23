
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

#pragma once
#include <base_Common.hpp>

namespace nfp {

    struct CharacterId {
        u16 game_character_id;
        u8 character_variant;
    } PACKED;
    static_assert(sizeof(CharacterId) == 3);

    struct AmiiboId {
        CharacterId character_id;
        u8 series;
        u16 model_number;
        u8 figure_type;
    } PACKED;
    static_assert(sizeof(AmiiboId) == 7);

    struct AdminInfo {
        u64 program_id;
        u32 access_id;
        u16 crc32_counter;
        u8 flags;
        u8 unk_0x2;
        u8 console_kind;
        u8 pad[7];
        u8 reserved[0x28];
    };
    static_assert(sizeof(AdminInfo) == 0x40);

    struct Date {
        u16 year;
        u8 month;
        u8 day;
    };
    static_assert(sizeof(Date) == 0x4);

    struct NfpData {
        u8 unk_0xA5;
        u8 unk_pad;
        u16 unk_u16;
        u32 unk_u32;
        u8 unk_pad_5[56];
        Date last_write_date;
        u16 unk_u16_2;
        u8 unk_u8_1;
        u8 unk_pad_2;
        u32 app_area_size;
        u8 unk_pad_3[4];
        u8 unk_data_pad[0x30];
        u8 legacy_mii[0x60];
        u8 unk_data[0x8];
        Date first_write_date;
        u16 name_utf16[11];
        u8 unk_u8_2;
        u8 unk_u8_3;
        u32 unk_u32_2;
        u64 unk_u64;
        u64 unk_u64_2;
        u32 unk_u32_3;
        u8 unk_pad_4[100];
        AdminInfo admin_info;
        u8 app_area[0xD8];
    };
    static_assert(sizeof(NfpData) == 0x298);

    Result Initialize();
    bool IsReady();
    Result Open();
    NfpTagInfo GetTagInfo();
    NfpRegisterInfo GetRegisterInfo();
    NfpCommonInfo GetCommonInfo();
    NfpModelInfo GetModelInfo();
    NfpData GetAll(); // No need for GetAdminInfo since NfpData includes it
    std::string ExportAsVirtualAmiibo(const NfpTagInfo &tag, const NfpRegisterInfo &reg, const NfpCommonInfo &common, const NfpModelInfo &model, const NfpData &all);
    void Close();
    void Exit();

}