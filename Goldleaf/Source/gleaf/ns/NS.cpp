#include <gleaf/ns/NS.hpp>

Service nsgisrv;
Service nsamsrv;
u64 nscnt = 0;

namespace gleaf::ns
{
    Result Initialize()
    {
        atomicIncrement64(&nscnt);
        Result rc = 0;
        if(!serviceIsActive(&nsgisrv) && !serviceIsActive(&nsamsrv))
        {
            if(kernelAbove300())
            {
                rc = smGetService(&nsgisrv, "ns:am2");
                if(R_FAILED(rc)) return rc;
                IpcCommand c;
                ipcInitialize(&c);
                struct Raw
                {
                    u64 Magic;
                    u64 CmdId;
                } *raw;
                raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
                raw->Magic = SFCI_MAGIC;
                raw->CmdId = 7996;
                rc = serviceIpcDispatch(&nsgisrv);
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
                    if(R_SUCCEEDED(rc)) serviceCreate(&nsamsrv, r.Handles[0]);
                }
            }
            else rc = smGetService(&nsamsrv, "ns:am");
        }
        return rc;
    }

    void Finalize()
    {
        if(atomicDecrement64(&nscnt) == 0)
        {
            serviceClose(&nsamsrv);
            if(kernelAbove300()) serviceClose(&nsgisrv);
        }
    }

    bool HasInitialized()
    {
        return serviceIsActive(&nsamsrv);
    }

    Result DeleteApplicationCompletely(u64 ApplicationId)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            u64 ApplicationId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 5;
        raw->ApplicationId = ApplicationId;
        Result rc = serviceIpcDispatch(&nsamsrv);
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

    Result PushApplicationRecord(u64 ApplicationId, u8 LastModifiedEvent, ContentStorageRecord *Records, size_t RecordsSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, Records, RecordsSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            u8 LastModifiedEvent;
            u8 Pad[0x7];
            u64 ApplicationId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 16;
        raw->LastModifiedEvent = LastModifiedEvent;
        raw->ApplicationId = ApplicationId;
        Result rc = serviceIpcDispatch(&nsamsrv);
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

    std::tuple<Result, u32> ListApplicationRecordContentMeta(u64 Offset, u64 ApplicationId, void *Out, size_t OutBufferSize)
    {
        u32 OutCount = 0;
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, Out, OutBufferSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            u64 Offset;
            u64 ApplicationId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 17;
        raw->ApplicationId = ApplicationId;
        raw->Offset = Offset;
        Result rc = serviceIpcDispatch(&nsamsrv);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u32 Count;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) OutCount = resp->Count;
        }
        return std::make_tuple(rc, OutCount);
    }

    Result DeleteApplicationRecord(u64 ApplicationId)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            u64 ApplicationId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 27;
        raw->ApplicationId = ApplicationId;
        Result rc = serviceIpcDispatch(&nsamsrv);
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

    std::tuple<Result, u32> CountApplicationContentMeta(u64 ApplicationId)
    {
        u32 OutCount = 0;
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            u64 ApplicationId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 600;
        raw->ApplicationId = ApplicationId;
        Result rc = serviceIpcDispatch(&nsamsrv);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u32 Count;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) OutCount = resp->Count;
        }
        return std::make_tuple(rc, OutCount);
    }
}