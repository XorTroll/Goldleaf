
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

#include <amssu/amssu_Service.hpp>

namespace amssu
{
    static Service amssu_srv;
    static TransferMemory amssu_tmem;

    Result Initialize()
    {
        if(serviceIsActive(&amssu_srv)) return 0;
        return smGetService(&amssu_srv, "ams:su");
    }

    void Exit()
    {
        if(serviceIsActive(&amssu_srv)) serviceClose(&amssu_srv);
        tmemClose(&amssu_tmem);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&amssu_srv);
    }

    #define IPC_PATH_MAKE(path) \
        char ipc_path[FS_MAX_PATH] = {}; \
        strncpy(ipc_path, path, FS_MAX_PATH - 1); \
        ipc_path[FS_MAX_PATH - 1] = 0;

    Result GetUpdateInformation(const char *path, UpdateInformation *out_info)
    {
        IPC_PATH_MAKE(path);
        return serviceDispatchOut(&amssu_srv, 0, *out_info,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
        );
    }

    Result ValidateUpdate(const char *path, UpdateValidationInfo *out_info)
    {
        IPC_PATH_MAKE(path);
        return serviceDispatchOut(&amssu_srv, 1, *out_info,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
        );
    }

    Result SetupUpdate(void *buf, size_t size, const char *path, bool exfat)
    {
        Result rc = 0;
        IPC_PATH_MAKE(path);

        if(buf == nullptr) rc = tmemCreate(&amssu_tmem, size, Perm_None);
        else rc = tmemCreateFromMemory(&amssu_tmem, buf, size, Perm_None);
        if(R_FAILED(rc)) return rc;

        const struct {
            u8 exfat;
            u64 size;
        } in = { exfat, amssu_tmem.size };

        rc = serviceDispatchIn(&amssu_srv, 2, in,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
            .in_num_handles = 1,
            .in_handles = { amssu_tmem.handle },
        );

        if(R_FAILED(rc)) tmemClose(&amssu_tmem);
        return rc;
    }

    Result SetupUpdateWithVariation(void *buf, size_t size, const char *path, bool exfat, u32 variation)
    {
        Result rc = 0;
        IPC_PATH_MAKE(path);

        if(buf == nullptr) rc = tmemCreate(&amssu_tmem, size, Perm_None);
        else rc = tmemCreateFromMemory(&amssu_tmem, buf, size, Perm_None);
        if(R_FAILED(rc)) return rc;

        const struct {
            u8 exfat;
            u32 variation;
            u64 size;
        } in = { exfat, variation, amssu_tmem.size };

        rc = serviceDispatchIn(&amssu_srv, 3, in,
            .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcPointer | SfBufferAttr_FixedSize },
            .buffers = { { ipc_path, FS_MAX_PATH } },
            .in_num_handles = 1,
            .in_handles = { amssu_tmem.handle },
        );

        if(R_FAILED(rc)) tmemClose(&amssu_tmem);
        return rc;
    }

    Result RequestPrepareUpdate(AsyncResult *async)
    {
        *async = {};

        Handle event = INVALID_HANDLE;
        Result rc = serviceDispatch(&amssu_srv, 4,
            .out_num_objects = 1,
            .out_objects = &async->s,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &event,
        );

        if(R_SUCCEEDED(rc)) eventLoadRemote(&async->event, event, false);
        return rc;
    }

    Result GetPrepareUpdateProgress(NsSystemUpdateProgress *out)
    {
        return serviceDispatchOut(&amssu_srv, 5, *out);
    }

    Result HasPreparedUpdate(bool *out_has)
    {
        return serviceDispatchOut(&amssu_srv, 6, *out_has);
    }

    Result ApplyPreparedUpdate()
    {
        return serviceDispatch(&amssu_srv, 7);
    }
}