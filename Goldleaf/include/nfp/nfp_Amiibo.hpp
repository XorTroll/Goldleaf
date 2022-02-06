
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
        u16 crc32_change_counter;
        u8 flags_c;
        u8 unk_8;
        u8 unk_8_2;
        u8 pad[7];
        u8 reserved[40];
    };
    static_assert(sizeof(AdminInfo) == 0x40);

    Result Initialize();
    bool IsReady();
    Result Open();
    NfpTagInfo GetTagInfo();
    NfpRegisterInfo GetRegisterInfo();
    NfpCommonInfo GetCommonInfo();
    NfpModelInfo GetModelInfo();
    AdminInfo GetAdminInfo();
    void DumpToEmuiibo(const NfpTagInfo &tag, const NfpRegisterInfo &reg, const NfpCommonInfo &common, const NfpModelInfo &model);
    void Close();
    void Exit();

}