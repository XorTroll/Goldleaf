
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

namespace acc
{
    Result DeleteUser(AccountUid uid)
    {
        return serviceDispatchIn(accountGetServiceSession(), 203, uid);
    }

    Result GetProfileEditor(AccountUid uid, Service *out_srv)
    {
        return serviceDispatchIn(accountGetServiceSession(), 205, uid,
            .out_num_objects = 1,
            .out_objects = out_srv,
        );
    }

    Result ProfileEditor_Store(Service *srv, AccountProfileBase base, AccountUserData udata)
    {
        return serviceDispatchIn(srv, 100, base,
            .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer },
            .buffers = { { &udata, sizeof(udata) } },
        );
    }

    Result ProfileEditor_StoreWithImage(Service *srv, AccountProfileBase base, AccountUserData udata, u8 *jpg, size_t jpgsize)
    {
        return serviceDispatchIn(srv, 101, base,
            .buffer_attrs = {
                SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { &udata, sizeof(udata) },
                { jpg, jpgsize },
            },
        );
    }

    Result GetBaasAccountAdministrator(AccountUid uid, Service *out_srv)
    {
        return serviceDispatchIn(accountGetServiceSession(), 250, uid,
            .out_num_objects = 1,
            .out_objects = out_srv,
        );
    }

    Result BaasAdministrator_IsLinkedWithNintendoAccount(Service *srv, bool *out)
    {
        return serviceDispatchOut(srv, 250, *out);
    }

    Result BaasAdministrator_DeleteRegistrationInfoLocally(Service *srv)
    {
        return serviceDispatch(srv, 203);
    }

    Result BaasAdministrator_GetAccountId(Service *srv, u64 *out_id)
    {
        return serviceDispatchOut(srv, 1, *out_id);
    }

    Result BaasAdministrator_GetNintendoAccountId(Service *srv, u64 *out_id)
    {
        return serviceDispatchOut(srv, 120, *out_id);
    }
}