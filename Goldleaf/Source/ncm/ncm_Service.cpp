#include <ncm/ncm_Service.hpp>

static Service ncmsrv;
static u64 ncmcnt;

namespace ncm
{
    Result Initialize()
    {
        Result rc = 0;
        atomicIncrement64(&ncmcnt);
        if(!serviceIsActive(&ncmsrv))
        {
            rc = smGetService(&ncmsrv, "ncm");
            if(R_SUCCEEDED(rc)) smAddOverrideHandle(smEncodeName("ncm"), ncmsrv.handle);
        }
        return rc;
    }

    void Finalize()
    {
        if(atomicDecrement64(&ncmcnt) == 0) serviceClose(&ncmsrv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&ncmsrv);
    }

    Result CreatePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, const NcmNcaId *RegisteredId, u64 Size)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            NcmNcaId PlaceHolderId;
            NcmNcaId RegisteredId;
            u64 size;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 1;
        raw->size = Size;
        memcpy(&raw->PlaceHolderId, PlaceHolderId, sizeof(NcmNcaId));
        memcpy(&raw->RegisteredId, RegisteredId, sizeof(NcmNcaId));
        Result rc = serviceIpcDispatch(&Storage->s);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
        }
        return rc;
    }

    Result DeletePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            NcmNcaId PlaceHolderId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 2;
        memcpy(&raw->PlaceHolderId, PlaceHolderId, sizeof(NcmNcaId));
        Result rc = serviceIpcDispatch(&Storage->s);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
        }
        return rc;
    }

    Result WritePlaceHolder(NcmContentStorage *Storage, const NcmNcaId *PlaceHolderId, u64 Offset, void *Buffer, size_t BufSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, Buffer, BufSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            NcmNcaId PlaceHolderId;
            u64 Offset;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 4;
        raw->Offset = Offset;
        memcpy(&raw->PlaceHolderId, PlaceHolderId, sizeof(NcmNcaId));
        Result rc = serviceIpcDispatch(&Storage->s);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
        }
        return rc;
    }
    
    Result Delete(NcmContentStorage *Storage, const NcmNcaId *RegisteredId)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            NcmNcaId PlaceHolderId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 6;
        memcpy(&raw->PlaceHolderId, RegisteredId, sizeof(NcmNcaId));
        Result rc = serviceIpcDispatch(&Storage->s);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
        }
        return rc;
    }

    Result ContentMetaDatabase_GetSize(NcmContentMetaDatabase *Database, const NcmMetaRecord *Record, u64 *out_Size)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            NcmMetaRecord Record;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 10;
        memcpy(&raw->Record, Record, sizeof(NcmMetaRecord));
        Result rc = serviceIpcDispatch(&Database->s);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u64 SizeOut;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) *out_Size = resp->SizeOut;
        }
        return rc;
    }
}