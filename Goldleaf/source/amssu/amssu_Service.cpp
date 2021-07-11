
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

#include <amssu/amssu_Service.hpp>

namespace amssu {

    namespace {

        Service g_AmsSuService;
        TransferMemory g_AmsSuTransferMemory;

    }

    Result Initialize() {
        if(serviceIsActive(&g_AmsSuService)) {
            return 0;
        }
        return smGetService(&g_AmsSuService, "ams:su");
    }

    void Exit() {
        if(serviceIsActive(&g_AmsSuService)) {
            serviceClose(&g_AmsSuService);
        }
        tmemClose(&g_AmsSuTransferMemory);
    }

    bool HasInitialized() {
        return serviceIsActive(&g_AmsSuService);
    }

    #define _AMSSU_MAKE_IPC_PATH(path) \
        char ipc_path[FS_MAX_PATH] = {}; \
        strncpy(ipc_path, path, FS_MAX_PATH - 1); \
        ipc_path[FS_MAX_PATH - 1] = 0;

    Result GetUpdateInformation(const char *path, UpdateInformation *out_info) {
        _AMSSU_MAKE_IPC_PATH(path);
        return serviceDispatchOut(&g_AmsSuService, 0, *out_info,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
        );
    }

    Result ValidateUpdate(const char *path, UpdateValidationInfo *out_info) {
        _AMSSU_MAKE_IPC_PATH(path);
        return serviceDispatchOut(&g_AmsSuService, 1, *out_info,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
        );
    }

    Result SetupUpdate(void *buf, size_t size, const char *path, bool exfat) {
        Result rc = 0;
        _AMSSU_MAKE_IPC_PATH(path);

        if(buf == nullptr) {
            rc = tmemCreate(&g_AmsSuTransferMemory, size, Perm_None);
        }
        else {
            rc = tmemCreateFromMemory(&g_AmsSuTransferMemory, buf, size, Perm_None);
        }
        if(R_FAILED(rc)) {
            return rc;
        }

        const struct {
            u8 exfat;
            u64 size;
        } in = { exfat, g_AmsSuTransferMemory.size };

        rc = serviceDispatchIn(&g_AmsSuService, 2, in,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
            .in_num_handles = 1,
            .in_handles = { g_AmsSuTransferMemory.handle },
        );

        if(R_FAILED(rc)) {
            tmemClose(&g_AmsSuTransferMemory);
        }
        return rc;
    }

    Result SetupUpdateWithVariation(void *buf, size_t size, const char *path, bool exfat, u32 variation) {
        Result rc = 0;
        _AMSSU_MAKE_IPC_PATH(path);

        if(buf == nullptr) {
            rc = tmemCreate(&g_AmsSuTransferMemory, size, Perm_None);
        }
        else {
            rc = tmemCreateFromMemory(&g_AmsSuTransferMemory, buf, size, Perm_None);
        }
        if(R_FAILED(rc)) {
            return rc;
        }

        const struct {
            u8 exfat;
            u32 variation;
            u64 size;
        } in = { exfat, variation, g_AmsSuTransferMemory.size };

        rc = serviceDispatchIn(&g_AmsSuService, 3, in,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
            .in_num_handles = 1,
            .in_handles = { g_AmsSuTransferMemory.handle },
        );

        if(R_FAILED(rc)) {
            tmemClose(&g_AmsSuTransferMemory);
        }
        return rc;
    }

    Result RequestPrepareUpdate(AsyncResult *async) {
        *async = {};

        auto event_h = INVALID_HANDLE;
        auto rc = serviceDispatch(&g_AmsSuService, 4,
            .out_num_objects = 1,
            .out_objects = &async->s,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &event_h,
        );

        if(R_SUCCEEDED(rc)) {
            eventLoadRemote(&async->event, event_h, false);
        }
        return rc;
    }

    Result GetPrepareUpdateProgress(NsSystemUpdateProgress *out) {
        return serviceDispatchOut(&g_AmsSuService, 5, *out);
    }

    Result HasPreparedUpdate(bool *out_has) {
        return serviceDispatchOut(&g_AmsSuService, 6, *out_has);
    }

    Result ApplyPreparedUpdate() {
        return serviceDispatch(&g_AmsSuService, 7);
    }

}