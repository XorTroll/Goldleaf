
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

#pragma once
#include <switch.h>
#include <es/es_Types.hpp>

namespace es
{
    Result Initialize();
    void Exit();
    bool HasInitialized();

    Result ImportTicket(const void *Ticket, size_t TicketSize, const void *Cert, size_t CertSize);
    Result DeleteTicket(const RightsId *RId, size_t RIdSize);
    Result GetTitleKey(const RightsId *RId, u8 *out_Key, size_t out_KeySize);
    Result CountCommonTicket(u32 *out_Count);
    Result CountPersonalizedTicket(u32 *out_Count);
    Result ListCommonTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize);
    Result ListPersonalizedTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize);
    Result GetCommonTicketData(const RightsId *RId, void *out_Data, size_t out_DataSize, u64 *out_Unk);
}