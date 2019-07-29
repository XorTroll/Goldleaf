#include <es/es_Service.hpp>
#include <cstring>

static Service essrv;
static u64 escnt = 0;

namespace es
{
    Result Initialize()
    {
        atomicIncrement64(&escnt);
        if(serviceIsActive(&essrv)) return 0;
        return smGetService(&essrv, "es");
    }

    void Finalize()
    {
        if(atomicDecrement64(&escnt) == 0) serviceClose(&essrv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&essrv);
    }

    Result ImportTicket(void const *Ticket, size_t TicketSize, void const *Cert, size_t CertSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, Ticket, TicketSize, BufferType_Normal);
        ipcAddSendBuffer(&c, Cert, CertSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 1;
        Result rc = serviceIpcDispatch(&essrv);
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

    Result DeleteTicket(const RightsId *RId, size_t RIdSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendBuffer(&c, RId, RIdSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 3;
        Result rc = serviceIpcDispatch(&essrv);
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

    Result GetTitleKey(const RightsId *RId, u8 *out_Key, size_t out_KeySize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, out_Key, out_KeySize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            RightsId RId;
            u32 KeyGen;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 8;
        raw->KeyGen = 0;
        memcpy(&raw->RId, RId, sizeof(RightsId));
        Result rc = serviceIpcDispatch(&essrv);
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

    Result CountCommonTicket(u32 *out_Count)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 9;
        Result rc = serviceIpcDispatch(&essrv);
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
            if(R_SUCCEEDED(rc)) *out_Count = resp->Count;
        }
        return rc;
    }

    Result CountPersonalizedTicket(u32 *out_Count)
    {
        IpcCommand c;
        ipcInitialize(&c);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 10;
        Result rc = serviceIpcDispatch(&essrv);
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
            if(R_SUCCEEDED(rc)) *out_Count = resp->Count;
        }
        return rc;
    }

    Result ListCommonTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, out_Ids, out_IdsSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 11;
        Result rc = serviceIpcDispatch(&essrv);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u32 Written;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) *out_Written = resp->Written;
        }
        return rc;
    }

    Result ListPersonalizedTicket(u32 *out_Written, RightsId *out_Ids, size_t out_IdsSize)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, out_Ids, out_IdsSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 12;
        Result rc = serviceIpcDispatch(&essrv);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u32 Written;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) *out_Written = resp->Written;
        }
        return rc;
    }

    Result GetCommonTicketData(const RightsId *RId, void *out_Data, size_t out_DataSize, u64 *out_Unk)
    {
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, out_Data, out_DataSize, BufferType_Normal);
        struct Raw
        {
            u64 Magic;
            u64 CmdId;
            RightsId RId;
        } *raw;
        raw = (struct Raw*)ipcPrepareHeader(&c, sizeof(*raw));
        raw->Magic = SFCI_MAGIC;
        raw->CmdId = 16;
        memcpy(&raw->RId, RId, sizeof(RightsId));
        Result rc = serviceIpcDispatch(&essrv);
        if(R_SUCCEEDED(rc))
        {
            IpcParsedCommand r;
            ipcParse(&r);
            struct Parsed
            {
                u64 Magic;
                u64 Result;
                u64 Unknown;
            } *resp = (struct Parsed*)r.Raw;
            rc = resp->Result;
            if(R_SUCCEEDED(rc)) *out_Unk = resp->Unknown;
        }
        return rc;
    }
}