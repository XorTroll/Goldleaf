
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

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
        struct {
            AccountProfileBase PBase;
        } in;
        memcpy(&in.PBase, PBase, sizeof(AccountProfileBase));

        return serviceDispatchIn(&this->srv, 100, in,
            .buffer_attrs = {
                SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In,
            },
            .buffers = {
                { UData, sizeof(AccountUserData) },
            },
        );
    }

    Result ProfileEditor::StoreWithImage(AccountProfileBase *PBase, AccountUserData *UData, u8 *JPEG, size_t JPEGSize)
    {
        struct {
            AccountProfileBase PBase;
        } in;
        memcpy(&in.PBase, PBase, sizeof(AccountProfileBase));

        return serviceDispatchIn(&this->srv, 101, in,
            .buffer_attrs = {
                SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { UData, sizeof(AccountUserData) },
                { JPEG, JPEGSize },
            },
        );
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

    Result BaasAdministrator::IsLinkedWithNintendoAccount(bool *LinkedOut)
    {
        struct {
            bool Linked;
        } out;

        Result rc = serviceDispatchOut(&this->srv, 250, out);

        if (R_SUCCEEDED(rc) && LinkedOut) *LinkedOut = out.Linked;
        
        return rc;
    }

    Result BaasAdministrator::DeleteRegistrationInfoLocally()
    {
        return serviceDispatch(&this->srv, 203);
    }

    Result BaasAdministrator::GetAccountId(u64 *out_AId)
    {
        struct {
            u64 AId;
        } out;

        Result rc = serviceDispatchOut(&this->srv, 1, out);

        if (R_SUCCEEDED(rc) && out_AId) *out_AId = out.AId;

        return rc;
    }

    Result BaasAdministrator::GetNintendoAccountId(u64 *out_NAId)
    {
        struct {
            u64 NAId;
        } out;

        Result rc = serviceDispatchOut(&this->srv, 120, out);

        if (R_SUCCEEDED(rc) && out_NAId) *out_NAId = out.NAId;

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

    Result DeleteUser(AccountUid UserId)
    {
        struct {
            AccountUid UserId;
        } in;
        memcpy(&in.UserId, &UserId, sizeof(AccountUid));

        return serviceDispatchIn(&susrv, 203, in);
    }

    Result GetProfileEditor(AccountUid UserId, ProfileEditor *out_Editor)
    {
        struct {
            AccountUid UserId;
        } in;
        memcpy(&in.UserId, &UserId, sizeof(AccountUid));

        Handle handle_out  = INVALID_HANDLE;
        Result rc = serviceDispatchIn(&susrv, 205, in,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle_out,
        );
        Service srv;
        serviceCreate(&srv, handle_out);
        *out_Editor = ProfileEditor(srv);

        return rc;
    }

    Result GetBaasAccountAdministrator(AccountUid UserId, BaasAdministrator *out_Admin)
    {
        struct {
            AccountUid UserId;
        } in;
        memcpy(&in.UserId, &UserId, sizeof(AccountUid));

        Handle handle_out = INVALID_HANDLE;
        Result rc = serviceDispatchIn(&susrv, 250, in,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle_out,
        );
        Service srv;
        serviceCreate(&srv, handle_out);
        *out_Admin = BaasAdministrator(srv);

        return rc;
    }
}