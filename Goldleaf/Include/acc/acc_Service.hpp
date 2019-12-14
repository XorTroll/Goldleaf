
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

#pragma once
#include <switch.h>

namespace acc
{
    Result DeleteUser(AccountUid uid);

    Result GetProfileEditor(AccountUid uid, Service *out_srv);
    Result ProfileEditor_Store(Service *srv, AccountProfileBase base, AccountUserData udata);
    Result ProfileEditor_StoreWithImage(Service *srv, AccountProfileBase base, AccountUserData udata, u8 *jpg, size_t jpgsize);

    Result GetBaasAccountAdministrator(AccountUid uid, Service *out_srv);
    Result BaasAdministrator_IsLinkedWithNintendoAccount(Service *srv, bool *out);
    Result BaasAdministrator_DeleteRegistrationInfoLocally(Service *srv);
    Result BaasAdministrator_GetAccountId(Service *srv, u64 *out_id);
    Result BaasAdministrator_GetNintendoAccountId(Service *srv, u64 *out_id);
}