
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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
#include "service_guard.h"

static Service g_esSrv;

namespace es
{
    NX_GENERATE_SERVICE_GUARD(es);
    
    Result _esInitialize(void) {
        return smGetService(&g_esSrv, "es");
    }

    void _esCleanup(void) {
        serviceClose(&g_esSrv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&g_esSrv);
    }

    Result ImportTicket(void const *Ticket, size_t TicketSize, void const *Cert, size_t CertSize)
    {
        return serviceDispatch(&g_esSrv, 1,
            .buffer_attrs = {
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { Ticket,   TicketSize },
                { Cert,  CertSize },
            },
        );
    }

    Result DeleteTicket(const RightsId *RId, size_t RIdSize)
    {
        return serviceDispatch(&g_esSrv, 3,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { RId, RIdSize }, },
        );
    }

    Result GetTitleKey(const RightsId *RId, u8 *out_Key, size_t out_KeySize)
    {
        struct {
            RightsId RId;
            u32 KeyGen; 
        } in;
        memcpy(&in.RId, RId, sizeof(RightsId));
        in.KeyGen = 0;
        return serviceDispatchIn(&g_esSrv, 8, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Key, out_KeySize } },
        );
    }

    Result CountCommonTicket(u32 *out_Count)
    {
        struct {
            u32 num_tickets;
        } out;

        Result rc = serviceDispatchOut(&g_esSrv, 9, out);
        if (R_SUCCEEDED(rc) && out_Count) *out_Count = out.num_tickets;
        
        return rc;
    }

    Result CountPersonalizedTicket(u32 *out_Count)
    {
        struct {
            u32 num_tickets;
        } out;

        Result rc = serviceDispatchOut(&g_esSrv, 10, out);
        if (R_SUCCEEDED(rc) && out_Count) *out_Count = out.num_tickets;
        
        return rc;
    }

    Result ListCommonTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        struct {
            u32 num_rights_ids_written;
        } out;
        
        Result rc = serviceDispatchOut(&g_esSrv, 11, out,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Ids, out_IdsSize } },
        );
        if (R_SUCCEEDED(rc) && out_Written) *out_Written = out.num_rights_ids_written;
        
        return rc;
    }

    Result ListPersonalizedTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        struct {
            u32 num_rights_ids_written;
        } out;
        
        Result rc = serviceDispatchOut(&g_esSrv, 12, out,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Ids, out_IdsSize } },
        );
        if (R_SUCCEEDED(rc) && out_Written) *out_Written = out.num_rights_ids_written;
        
        return rc;
    }

    Result GetCommonTicketData(const RightsId *RId, void *out_Data, size_t out_DataSize, u64 *out_Unk)
    {
        struct {
            RightsId rightsId;
        } in;
        memcpy(&in.rightsId, RId, sizeof(RightsId));

        struct {
            u64 unk;
        } out;

        Result rc = serviceDispatchInOut(&g_esSrv, 16, in, out,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_Data, out_DataSize } },
        );
        return rc;
    }
}