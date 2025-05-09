
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

    struct CharacterId {
        u16 game_character_id;
        u8 character_variant;
    } NX_PACKED;
    static_assert(sizeof(CharacterId) == 3);

    struct AmiiboId {
        CharacterId character_id;
        u8 series;
        u16 model_number;
        u8 figure_type;
    } NX_PACKED;
    static_assert(sizeof(AmiiboId) == 7);

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
