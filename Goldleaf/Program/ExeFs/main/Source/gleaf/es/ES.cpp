#include <gleaf/es/ES.hpp>

Service essrv;
u64 escnt = 0;

namespace gleaf::es
{
    Result Initialize()
    {
        atomicIncrement64(&escnt);
        Result rc = 0;
        if(!serviceIsActive(&essrv)) rc = smGetService(&essrv, "es");
        return rc;
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

    std::tuple<Result, u8*, size_t> GetTitleKey(const RightsId *RId)
    {
        u8 *Out = NULL;
        size_t OutSize = 0;
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, Out, OutSize, BufferType_Normal);
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
        return std::make_tuple(rc, Out, OutSize);
    }

    std::tuple<Result, u8> CountCommonTicket()
    {
        u8 OutCount = 0;
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
            if(R_SUCCEEDED(rc)) OutCount = resp->Count;
        }
        return std::make_tuple(rc, OutCount);
    }

    std::tuple<Result, u8> CountPersonalizedTicket()
    {
        u8 OutCount = 0;
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
            if(R_SUCCEEDED(rc)) OutCount = resp->Count;
        }
        return std::make_tuple(rc, OutCount);
    }

    std::tuple<Result, u32, RightsId*> ListCommonTicket(size_t Size)
    {
        u32 OutWrittenRIds = 0;
        RightsId *OutRIds = (RightsId*)malloc(Size);
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, OutRIds, Size, BufferType_Normal);
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
            if(R_SUCCEEDED(rc)) OutWrittenRIds = resp->Written;
        }
        return std::make_tuple(rc, OutWrittenRIds, OutRIds);
    }

    std::tuple<Result, u32, RightsId*> ListPersonalizedTicket(size_t Size)
    {
        u32 OutWrittenRIds = 0;
        RightsId *OutRIds = (RightsId*)malloc(Size);
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, OutRIds, Size, BufferType_Normal);
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
            if(R_SUCCEEDED(rc)) OutWrittenRIds = resp->Written;
        }
        return std::make_tuple(rc, OutWrittenRIds, OutRIds);
    }

    std::tuple<Result, u64, void*, size_t> GetCommonTicketData(const RightsId *RId)
    {
        u64 OutUnknown = 0;
        void *Out = NULL;
        size_t OutSize = 0;
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvBuffer(&c, Out, OutSize, BufferType_Normal);
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
            if(R_SUCCEEDED(rc)) OutUnknown = resp->Unknown;
        }
        return std::make_tuple(rc, OutUnknown, Out, OutSize);
    }
}