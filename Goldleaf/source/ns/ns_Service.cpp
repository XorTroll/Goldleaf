
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

#include <ns/ns_Service.hpp>

namespace ns {

    namespace {

        inline Service *GetApplicationManagerInterfaceService(Service *srv_ref) {
            if(hosversionAtLeast(3,0,0)) {
                nsGetApplicationManagerInterface(srv_ref);
                return srv_ref;
            }
            else {
                return nsGetServiceSession_ApplicationManagerInterface();
            }
        }

        inline void DisposeApplicationManagerInterfaceService(Service *srv_ref) {
            if(hosversionAtLeast(3,0,0)) {
                serviceClose(srv_ref);
            }
        }

    }

    Result PushApplicationRecord(const u64 app_id, const u8 last_modified_event, const ContentStorageMetaKey *cnt_storage_record_buf, const size_t cnt_storage_record_buf_count) {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);

        const struct {
            u8 last_modified_event;
            u8 pad[7];
            u64 app_id;
        } in = { last_modified_event, {}, app_id };
        
        auto rc = serviceDispatchIn(use_srv, 16, in,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { cnt_storage_record_buf, cnt_storage_record_buf_count * sizeof(ContentStorageMetaKey) } },
        );
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result ListApplicationRecordContentMeta(const u64 offset, const u64 app_id, ContentStorageMetaKey *out_buf, const size_t out_buf_count, u32 *out_count) {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);

        const struct {
            u64 offset;
            u64 app_id;
        } in = { offset, app_id };

        auto rc = serviceDispatchInOut(use_srv, 17, in, *out_count,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_buf, out_buf_count * sizeof(ContentStorageMetaKey) } },
        );
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result DeleteApplicationRecord(const u64 app_id) {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);

        auto rc = serviceDispatchIn(use_srv, 27, app_id);
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

    Result PushLaunchVersion(const u64 app_id, const u32 launch_version) {
        Service srv;
        auto use_srv = GetApplicationManagerInterfaceService(&srv);

        const struct {
            u32 version;
            u64 app_id;
            u8 pad[4];
        } in = { launch_version, app_id, {} };
        auto rc = serviceDispatchIn(use_srv, 36, in);
        DisposeApplicationManagerInterfaceService(&srv);
        return rc;
    }

}