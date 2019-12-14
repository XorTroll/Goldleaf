
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

namespace ns
{
    Result DeleteApplicationCompletely(u64 ApplicationId)
    {
        return serviceDispatchIn(nsGetServiceSession_ApplicationManagerInterface(), 5, ApplicationId);
    }

    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize)
    {
        struct
        {
            u8 last_modified_event;
            u8 padding[0x7];
            u64 appId;
        } in = { LastModifiedEvent, {0}, ApplicationId };
        
        return serviceDispatchIn(nsGetServiceSession_ApplicationManagerInterface(), 16, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { Records, RecordsSize } },
        );
    }

    Result ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, void *Out, size_t OutBufferSize, u32 *out_Count)
    {
        struct
        {
            u64 offset;
            u64 appId;
        } in = { Offset, ApplicationId };

        return serviceDispatchInOut(nsGetServiceSession_ApplicationManagerInterface(), 17, in, *out_Count,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { Out, OutBufferSize } },
        );
    }

    Result DeleteApplicationRecord(u64 ApplicationId)
    {
        return serviceDispatchIn(nsGetServiceSession_ApplicationManagerInterface(), 27, ApplicationId);
    }

    Result CountApplicationContentMeta(u64 ApplicationId, u32 *out_Count)
    {
        return serviceDispatchInOut(nsGetServiceSession_ApplicationManagerInterface(), 600, ApplicationId, *out_Count);
    }

    Result PushLaunchVersion(u64 ApplicationId, u32 LaunchVersion)
    {
        struct
        {
            u64 appId;
            u32 version;
            u32 padding;
        } in = { ApplicationId, LaunchVersion, 0 };
        
        return serviceDispatchIn(nsGetServiceSession_ApplicationManagerInterface(), 36, in);
    }
}