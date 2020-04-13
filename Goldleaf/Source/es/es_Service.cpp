
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

#include <es/es_Service.hpp>
#include <cstring>

namespace es
{
    static Service es_srv;

    Result Initialize()
    {
        if(serviceIsActive(&es_srv)) return 0;
        return smGetService(&es_srv, "es");
    }

    void Exit()
    {
        if(serviceIsActive(&es_srv)) serviceClose(&es_srv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&es_srv);
    }

    Result ImportTicket(const void *Ticket, size_t TicketSize, const void *Cert, size_t CertSize)
    {
        return serviceDispatch(&es_srv, 1,
            .buffer_attrs = {
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { Ticket, TicketSize },
                { Cert, CertSize },
            },
        );
    }

    Result DeleteTicket(const RightsId *RId, size_t RIdSize)
    {
        return serviceDispatch(&es_srv, 3,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { RId, RIdSize }, },
        );
    }

    Result GetTitleKey(const RightsId *RId, u8 *out_Key, size_t out_KeySize)
    {
        struct
        {
            RightsId RId;
            u32 KeyGen; 
        } in;
        memcpy(&in.RId, RId, sizeof(RightsId));
        in.KeyGen = 0;

        return serviceDispatchIn(&es_srv, 8, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Key, out_KeySize } },
        );
    }

    Result CountCommonTicket(u32 *out_Count)
    {
        return serviceDispatchOut(&es_srv, 9, *out_Count);
    }

    Result CountPersonalizedTicket(u32 *out_Count)
    {
        return serviceDispatchOut(&es_srv, 10, *out_Count);
    }

    Result ListCommonTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        return serviceDispatchOut(&es_srv, 11, *out_Written,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Ids, out_IdsSize } },
        );
    }

    Result ListPersonalizedTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        return serviceDispatchOut(&es_srv, 12, *out_Written,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Ids, out_IdsSize } },
        );
    }

    Result GetCommonTicketData(const RightsId *RId, void *out_Data, size_t out_DataSize, u64 *out_Unk)
    {
        return serviceDispatchInOut(&es_srv, 16, *RId, *out_Unk,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Data, out_DataSize } },
        );
    }
}