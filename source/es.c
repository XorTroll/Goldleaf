#include "es.h"

#include <string.h>
#include <switch.h>
#include <switch/arm/atomics.h>

static Service g_esSrv;
static u64 g_esRefCnt;

Result esInitialize() {
    atomicIncrement64(&g_esRefCnt);
    Result rc = smGetService(&g_esSrv, "es");
    return rc;
}

void esExit() {
    if (atomicDecrement64(&g_esRefCnt) == 0) {
        serviceClose(&g_esSrv);
    }
}

Result esGetTitleKey(u64 titleId, u8 *outBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u64 unk1;
        u64 unk2;
        u32 unk3;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 8;
    //raw->title_id = titleId;
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}