#include "nx/ipc/es.h"

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

Result esImportTicket(void const *tikBuf, size_t tikSize, void const *certBuf, size_t certSize)
{
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddSendBuffer(&c, tikBuf, tikSize, BufferType_Normal);
    ipcAddSendBuffer(&c, certBuf, certSize, BufferType_Normal);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
    
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

Result esDeleteTicket(const RightsId *rightsIdBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddSendBuffer(&c, rightsIdBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 3;
    
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

Result esGetTitleKey(const RightsId *rightsId, u8 *outBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
        RightsId rights_id;
        u32 key_generation;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 8;
    raw->key_generation = 0;
    memcpy(&raw->rights_id, rightsId, sizeof(RightsId));
    
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

Result esCountCommonTicket(u32 *numTickets)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 9;
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 num_tickets;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (numTickets) *numTickets = resp->num_tickets;
        }
    }
    
    return rc;  
}

Result esCountPersonalizedTicket(u32 *numTickets)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 10;
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 num_tickets;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (numTickets) *numTickets = resp->num_tickets;
        }
    }
    
    return rc;  
}

Result esListCommonTicket(u32 *numRightsIdsWritten, RightsId *outBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 11;
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 num_rights_ids_written;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (numRightsIdsWritten) *numRightsIdsWritten = resp->num_rights_ids_written;
        }
    }
    
    return rc;
}

Result esListPersonalizedTicket(u32 *numRightsIdsWritten, RightsId *outBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 12;
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 num_rights_ids_written;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (numRightsIdsWritten) *numRightsIdsWritten = resp->num_rights_ids_written;
        }
    }
    
    return rc;
}

Result esGetCommonTicketData(u64 *unkOut, void *outBuf1, size_t bufSize1, const RightsId* rightsId)
{
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf1, bufSize1, BufferType_Normal);

    struct {
        u64 magic;
        u64 cmd_id;
        RightsId rights_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 16;
    memcpy(&raw->rights_id, rightsId, sizeof(RightsId));
    
    Result rc = serviceIpcDispatch(&g_esSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u64 unk;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (unkOut) *unkOut = resp->unk;
        }
    }
    
    return rc;
}