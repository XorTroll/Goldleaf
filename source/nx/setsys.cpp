// Copied from SwitchIdent (https://github.com/joel16/SwitchIdent)

#include <string.h>

#include "nx/setsys.hpp"

typedef struct {
    u64 magic;
    u64 cmd_id;
} IpcCommandHeader;

typedef struct {
    u64 magic;
    u64 result;
} IpcVersionResponse;

static Result GetFirmwareVersion(Service *srv, SetSysFirmwareVersion *ver) {
    char buffer[0x100];
    size_t size = sizeof(buffer);
    memset(buffer, 0, size);

    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvStatic(&c, buffer, size, 0);

    IpcCommandHeader * raw;

    raw = (IpcCommandHeader *) ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 3;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        IpcVersionResponse * resp = (IpcVersionResponse *) r.Raw;
        rc = resp->result;

        if (R_SUCCEEDED(rc)){
            memcpy(ver, &buffer, sizeof(buffer));
        }
    }

    return rc;
}

static Result GetFirmwareVersion2(Service *srv, SetSysFirmwareVersion *ver) {
    char buffer[0x100];
    size_t size = sizeof(buffer);
    memset(buffer, 0, size);

    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvStatic(&c, buffer, size, 0);

    IpcCommandHeader * raw;

    raw = (IpcCommandHeader *) ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 4;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        IpcVersionResponse * resp = (IpcVersionResponse *) r.Raw;
        rc = resp->result;

        if (R_SUCCEEDED(rc)){
            memcpy(ver, &buffer, sizeof(buffer));
        }
    }

    return rc;
}

Result setsysGetFirmwareVersion(Service *srv, SetSysFirmwareVersion *ver) {
	if (kernelAbove200())
		return GetFirmwareVersion2(srv, ver);
	return GetFirmwareVersion(srv, ver);
}
