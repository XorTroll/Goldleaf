
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

#include <ns/ns_Service.hpp>

namespace ns
{
    static inline Service *GetApplicationManagerInterfaceService(Service *ptr)
    {
        if(hosversionAtLeast(3,0,0))
        {
            nsGetApplicationManagerInterface(ptr);
            return ptr;
        }
        return nsGetServiceSession_ApplicationManagerInterface();
    }

    static inline void DisposeApplicationManagerInterfaceService(Service *ptr)
    {
        if(hosversionAtLeast(3,0,0))
        {
            serviceClose(ptr);
        }
    }

    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize)
    {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);
        const struct
        {
            u8 last_modified_event;
            u8 padding[7];
            u64 app_id;
        } in = { LastModifiedEvent, {0}, ApplicationId };
        
        auto rc = serviceDispatchIn(use_srv, 16, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { Records, RecordsSize } },
        );
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, void *Out, size_t OutBufferSize, u32 *out_Count)
    {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);
        const struct
        {
            u64 offset;
            u64 app_id;
        } in = { Offset, ApplicationId };

        auto rc = serviceDispatchInOut(use_srv, 17, in, *out_Count,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { Out, OutBufferSize } },
        );
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result DeleteApplicationRecord(u64 ApplicationId)
    {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);
        auto rc = serviceDispatchIn(use_srv, 27, ApplicationId);
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result PushLaunchVersion(u64 ApplicationId, u32 LaunchVersion)
    {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);
        const struct
        {
            u32 version;
            u64 app_id;
            u8 pad[4];
        } in = { LaunchVersion, ApplicationId, {0} };
        auto rc = serviceDispatchIn(use_srv, 36, in);
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }
}