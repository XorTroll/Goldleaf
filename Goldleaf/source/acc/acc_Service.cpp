
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

namespace acc {

    Result DeleteUser(const AccountUid user_id) {
        return serviceDispatchIn(accountGetServiceSession(), 203, user_id);
    }

    Result GetProfileEditor(const AccountUid user_id, Service *out_editor_srv) {
        return serviceDispatchIn(accountGetServiceSession(), 205, user_id,
            .out_num_objects = 1,
            .out_objects = out_editor_srv,
        );
    }

    Result ProfileEditor_Store(Service *editor_srv, const AccountProfileBase prof_base, const AccountUserData user_data) {
        return serviceDispatchIn(editor_srv, 100, prof_base,
            .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer },
            .buffers = { { &user_data, sizeof(user_data) } },
        );
    }

    Result ProfileEditor_StoreWithImage(Service *editor_srv, const AccountProfileBase prof_base, const AccountUserData user_data, const u8 *jpg, const size_t jpg_size) {
        return serviceDispatchIn(editor_srv, 101, prof_base,
            .buffer_attrs = {
                SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { &user_data, sizeof(user_data) },
                { jpg, jpg_size },
            },
        );
    }

    Result GetBaasAccountAdministrator(const AccountUid user_id, Service *out_admin_srv) {
        return serviceDispatchIn(accountGetServiceSession(), 250, user_id,
            .out_num_objects = 1,
            .out_objects = out_admin_srv,
        );
    }

    Result BaasAdministrator_IsLinkedWithNintendoAccount(Service *admin_srv, bool *out_linked) {
        return serviceDispatchOut(admin_srv, 250, *out_linked);
    }

    Result BaasAdministrator_DeleteRegistrationInfoLocally(Service *admin_srv) {
        return serviceDispatch(admin_srv, 203);
    }

    Result BaasAdministrator_GetAccountId(Service *admin_srv, u64 *out_id) {
        return serviceDispatchOut(admin_srv, 1, *out_id);
    }

    Result BaasAdministrator_GetNintendoAccountId(Service *admin_srv, u64 *out_id) {
        return serviceDispatchOut(admin_srv, 120, *out_id);
    }
}