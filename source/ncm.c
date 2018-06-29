#include "ncm.h"

#include <string.h>
#include <switch.h>
#include <switch/arm/atomics.h>

static Service g_ncmSrv;

static u64 g_ncmRefCnt;

Result ncmInitialize() {
    atomicIncrement64(&g_ncmRefCnt);
    Result rc = smGetService(&g_ncmSrv, "ncm");
    return rc;
}

void ncmExit() {
    if (atomicDecrement64(&g_ncmRefCnt) == 0) {
        serviceClose(&g_ncmSrv);
    }
}

Result ncmOpenContentStorage(FsStorageId storage, NCMContentStorage* out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u32 storage_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 4;
    raw->storage_id = (u32)storage;
    
    Result rc = serviceIpcDispatch(&g_ncmSrv);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(&out->s, r.Handles[0]);
        }
    }
    
    return rc;
}

Result ncmContentStorageGetPath(NCMContentStorage* cs, const NCMNCAID* ncaId, char* out) {
    char out_path[FS_MAX_PATH] = {0};
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvStatic(&c, out_path, FS_MAX_PATH, 0);
    
    struct {
        u64 magic;
        u64 cmd_id;
        NCMNCAID ncaId;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 8;
    memcpy(&raw->ncaId, ncaId, sizeof(NCMNCAID));
    
    Result rc = serviceIpcDispatch(&cs->s);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            strncpy(out, out_path, FS_MAX_PATH);
        }
    }
    
    return rc;
}

Result ncmContentStorageGetSize(NCMContentStorage* cs, const NCMNCAID* ncaId, u64* out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        NCMNCAID ncaId;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 14;
    memcpy(&raw->ncaId, ncaId, sizeof(NCMNCAID));
    
    Result rc = serviceIpcDispatch(&cs->s);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u64 size;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            *out = resp->size;
        }
    }
    
    return rc;
}

Result ncmContentStorageReadContentIdFile(NCMContentStorage* cs, const NCMNCAID* ncaId, u64 offset, char* outBuf, size_t bufSize) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, outBuf, bufSize, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
        NCMNCAID ncaId;
        u64 offset;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 18;
    memcpy(&raw->ncaId, ncaId, sizeof(NCMNCAID));
    raw->offset = offset;
    
    Result rc = serviceIpcDispatch(&cs->s);

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

Result ncmContentStorageHas(NCMContentStorage* cs, const NCMNCAID *ncaId, bool *out)
{
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        NCMNCAID ncaId;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 7;
    memcpy(&raw->ncaId, ncaId, sizeof(NCMNCAID));
    
    Result rc = serviceIpcDispatch(&cs->s);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            bool out;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (out) *out = resp->out;
        }
    }
    
    return rc;
}
 
Result ncmOpenContentMetaDatabase(FsStorageId storage, NCMContentMetaDatabase* out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u32 storage_id; // Actually u8
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 5;
    raw->storage_id = (u32)storage;
    
    Result rc = serviceIpcDispatch(&g_ncmSrv);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(&out->s, r.Handles[0]);
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseSet(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u64 content_record_size, NCMContentRecord *content_records_in) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddSendBuffer(&c, content_records_in, content_record_size, BufferType_Normal);

    struct {
        u64 magic;
        u64 cmd_id;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 0;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
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

Result ncmContentMetaDatabaseGet(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u64 content_record_size, NCMContentRecord *content_records_out, u64 *size_read_out) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, content_records_out, content_record_size, BufferType_Normal);

    struct {
        u64 magic;
        u64 cmd_id;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u64 size_read;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (size_read_out) *size_read_out = resp->size_read;
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseRemove(NCMContentMetaDatabase* db, const NCMMetaRecord *record) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 2;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
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

Result ncmContentMetaDatabaseListContentInfo(NCMContentMetaDatabase* db, const NCMMetaRecord *record, u32 index, NCMContentRecord *content_records_out, size_t out_buf_size, u32 *entries_read_out) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, content_records_out, out_buf_size, BufferType_Normal);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 index;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 4;
    raw->index = index;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 entries_read;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (entries_read_out) *entries_read_out = resp->entries_read;
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseGetLatestContentMetaKey(NCMContentMetaDatabase* db, u64 titleID, NCMMetaRecord* out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u64 title_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 6;
    raw->title_id = titleID;
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            NCMMetaRecord record;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            memcpy(out, &resp->record, sizeof(NCMMetaRecord));
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseGetContentIdByType(NCMContentMetaDatabase* db, NCMContentType contentType, const NCMMetaRecord* record, NCMNCAID* out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u32 contentType;
        u32 padding0;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 3;
    raw->contentType = contentType;
    raw->padding0 = 0;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            NCMNCAID ncaId;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            memcpy(out, &resp->ncaId, sizeof(NCMNCAID));
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseListApplication(NCMContentMetaDatabase* db, u8 filter, NCMApplicationContentMetaKey *entries_buf_out, size_t entries_buf_size, u32 *num_entries_written_out, u32 *num_entries_total_out) {
    IpcCommand c;
    ipcInitialize(&c);
    ipcAddRecvBuffer(&c, entries_buf_out, entries_buf_size, BufferType_Normal);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u8  filter;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 7;
    raw->filter = filter;
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 entries_written;
            u32 total_entries;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (num_entries_written_out) *num_entries_written_out = resp->entries_written;
            if (num_entries_total_out) *num_entries_total_out = resp->total_entries;
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseHas(NCMContentMetaDatabase* db, const NCMMetaRecord *record, bool *out)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 8;
    memcpy(&raw->metaRecord, record, sizeof(NCMMetaRecord));
    
    Result rc = serviceIpcDispatch(&db->s);
    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            bool out;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            if (out) *out = resp->out;
        }
    }
    
    return rc;
}

Result ncmContentMetaDatabaseCommit(NCMContentMetaDatabase* db)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        NCMMetaRecord metaRecord;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 15;
    
    Result rc = serviceIpcDispatch(&db->s);
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