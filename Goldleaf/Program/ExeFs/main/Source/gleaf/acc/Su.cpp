#include <gleaf/acc/Su.hpp>

Service susrv;
u64 sucnt = 0;

namespace gleaf::acc
{
    ProfileEditor::ProfileEditor(Service Srv)
    {
        this->srv = Srv;
    }

    Result ProfileEditor::Store(AccountProfileBase *PBase, AccountUserData *UData)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        ipcAddSendStatic(&cmd, UData, sizeof(AccountUserData), 0);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
            AccountProfileBase PBase;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 100;
        memcpy(&iraw->PBase, PBase, sizeof(AccountProfileBase));
        Result rc = serviceIpcDispatch(&this->srv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
        }
        return rc;
    }

    Result ProfileEditor::StoreWithImage(AccountProfileBase *PBase, AccountUserData *UData, u8 *JPEG, size_t JPEGSize)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        ipcAddSendStatic(&cmd, UData, sizeof(AccountUserData), 0);
        ipcAddSendBuffer(&cmd, JPEG, JPEGSize, BufferType_Normal);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
            AccountProfileBase PBase;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 101;
        memcpy(&iraw->PBase, PBase, sizeof(AccountProfileBase));
        Result rc = serviceIpcDispatch(&this->srv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
        }
        return rc;
    }

    void ProfileEditor::Close()
    {
        serviceClose(&this->srv);
    }

    Result Initialize()
    {
        atomicIncrement64(&sucnt);
        Result rc = 0;
        if(!serviceIsActive(&susrv)) rc = smGetService(&susrv, "acc:su");
        return rc;
    }

    void Finalize()
    {
        if(atomicDecrement64(&sucnt) == 0) serviceClose(&susrv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&susrv);
    }

    Result DeleteUser(u128 UserId)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
            u128 UserId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 203;
        iraw->UserId = UserId;
        Result rc = serviceIpcDispatch(&susrv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
        }
        return rc;
    }

    std::tuple<Result, ProfileEditor*> GetProfileEditor(u128 UserId)
    {
        ProfileEditor *prf;
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
            u128 UserId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 205;
        iraw->UserId = UserId;
        Result rc = serviceIpcDispatch(&susrv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
            if(rc == 0)
            {
                Service srv;
                serviceCreate(&srv, pcmd.Handles[0]);
                prf = new ProfileEditor(srv);
            }
        }
        return std::make_tuple(rc, prf);
    }
}