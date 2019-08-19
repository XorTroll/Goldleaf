#include <acc/acc_Service.hpp>
#include <cstring>

static Service susrv;
static u64 sucnt = 0;

namespace acc
{
    ProfileEditor::ProfileEditor()
    {
    }

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

    BaasAdministrator::BaasAdministrator()
    {
    }

    BaasAdministrator::BaasAdministrator(Service Srv)
    {
        this->srv = Srv;
    }

    Result BaasAdministrator::IsLinkedWithNintendoAccount(bool *out)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 250;
        Result rc = serviceIpcDispatch(&this->srv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
                bool Linked;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
            if(R_SUCCEEDED(rc)) *out = oraw->Linked;
        }
        return rc;
    }

    Result BaasAdministrator::DeleteRegistrationInfoLocally()
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 203;
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

    Result BaasAdministrator::GetAccountId(u64 *out_AId)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 1;
        Result rc = serviceIpcDispatch(&this->srv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
                u64 AId;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
            if(R_SUCCEEDED(rc)) *out_AId = oraw->AId;
        }
        return rc;
    }

    Result BaasAdministrator::GetNintendoAccountId(u64 *out_NAId)
    {
        IpcCommand cmd;
        ipcInitialize(&cmd);
        struct InRaw
        {
            u64 Magic;
            u64 CommandId;
        } *iraw = (InRaw*)ipcPrepareHeader(&cmd, sizeof(*iraw));
        iraw->Magic = SFCI_MAGIC;
        iraw->CommandId = 120;
        Result rc = serviceIpcDispatch(&this->srv);
        if(rc == 0)
        {
            IpcParsedCommand pcmd;
            ipcParse(&pcmd);
            struct OutRaw
            {
                u64 Magic;
                u64 ResultCode;
                u64 NAId;
            } *oraw = (OutRaw*)pcmd.Raw;
            rc = oraw->ResultCode;
            if(R_SUCCEEDED(rc)) *out_NAId = oraw->NAId;
        }
        return rc;
    }

    void BaasAdministrator::Close()
    {
        serviceClose(&this->srv);
    }

    Result Initialize()
    {
        atomicIncrement64(&sucnt);
        if(serviceIsActive(&susrv)) return 0;
        return smGetService(&susrv, "acc:su");
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

    Result GetProfileEditor(u128 UserId, ProfileEditor *out_Editor)
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
                *out_Editor = ProfileEditor(srv);
            }
        }
        return rc;
    }

    Result GetBaasAccountAdministrator(u128 UserId, BaasAdministrator *out_Admin)
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
        iraw->CommandId = 250;
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
                *out_Admin = BaasAdministrator(srv);
            }
        }
        return rc;
    }
}