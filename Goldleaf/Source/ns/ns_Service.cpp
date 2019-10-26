
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

#include <ns/ns_Service.hpp>
#include "service_guard.h"

static Service g_nsGetterSrv;
static Service g_nsAppManSrv;

namespace ns
{

    static Result _nsextGetSession(Service* srv, Service* srv_out, u32 cmd_id);

    NX_GENERATE_SERVICE_GUARD(nsext);

    Result _nsextInitialize(void) {
        Result rc=0;

        if(hosversionBefore(3,0,0))
            return smGetService(&g_nsAppManSrv, "ns:am");

        rc = smGetService(&g_nsGetterSrv, "ns:am2");//TODO: Support the other services?(Only useful when ns:am2 isn't accessible)
        if (R_FAILED(rc)) return rc;

        rc = _nsextGetSession(&g_nsGetterSrv, &g_nsAppManSrv, 7996);

        if (R_FAILED(rc)) serviceClose(&g_nsGetterSrv);

        return rc;
    }

    void _nsextCleanup(void) {
        serviceClose(&g_nsAppManSrv);
        if(hosversionBefore(3,0,0)) return;

        serviceClose(&g_nsGetterSrv);
    }

    static Result _nsextGetSession(Service* srv, Service* srv_out, u32 cmd_id) {
        return serviceDispatch(srv, cmd_id,
            .out_num_objects = 1,
            .out_objects = srv_out,
        );
    }

    bool HasInitialized()
    {
        return serviceIsActive(&g_nsAppManSrv);
    }

    Result DeleteApplicationCompletely(u64 ApplicationId)
    {
        struct {
            u64 title_id;
        } in = { ApplicationId };
        return serviceDispatchIn(&g_nsAppManSrv, 5, in);
    }

    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize)
    {
        struct {
            u8 last_modified_event;
            u8 padding[0x7];
            u64 title_id;
        } in = { LastModifiedEvent, {0}, ApplicationId };
        
        return serviceDispatchIn(&g_nsAppManSrv, 16, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { Records, RecordsSize } });
    }

    Result ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, void *Out, size_t OutBufferSize, u32 *out_Count)
    {
        struct {
            u64 offset;
            u64 titleID;
        } in = { Offset, ApplicationId };

        struct {
            u32 entries_read;
        } out;

        Result rc = serviceDispatchInOut(&g_nsAppManSrv, 17, in, out,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { Out, OutBufferSize } });
        
        if (R_SUCCEEDED(rc) && out_Count) *out_Count = out.entries_read;

        return rc;
    }

    Result DeleteApplicationRecord(u64 ApplicationId)
    {
        struct {
            u64 titleID;
        } in = { ApplicationId };
        
        return serviceDispatchIn(&g_nsAppManSrv, 27, in);
    }

    Result CountApplicationContentMeta(u64 ApplicationId, u32 *out_Count)
    {
        struct {
            u64 titleId;
        } in = { ApplicationId };

        struct {
            u32 count;
        } out;

        Result rc = serviceDispatchInOut(&g_nsAppManSrv, 600, in, out);

        if (R_SUCCEEDED(rc) && out_Count) *out_Count = out.count;

        return rc;
    }

    Result PushLaunchVersion(u64 ApplicationId, u32 LaunchVersion)
    {
        struct {
            u64 titleID;
            u32 version;
            u32 padding;
        } in = { ApplicationId, LaunchVersion, 0 };
        
        return serviceDispatchIn(&g_nsAppManSrv, 36, in);
    }
}