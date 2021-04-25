
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

#pragma once
#include <es/es_Types.hpp>

namespace es {

    Result Initialize();
    void Exit();
    bool HasInitialized();

    Result ImportTicket(const void *tik, size_t tik_size, const void *cert, size_t cert_size);
    Result DeleteTicket(const RightsId &rights_id);
    Result CountCommonTicket(u32 *out_count);
    Result CountPersonalizedTicket(u32 *out_count);
    Result ListCommonTicket(u32 *out_written, RightsId *out_rights_id_buf, size_t out_rights_id_buf_size);
    Result ListPersonalizedTicket(u32 *out_written, RightsId *out_rights_id_buf, size_t out_rights_id_buf_size);

}