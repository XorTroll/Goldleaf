
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
    class ProfileEditor
    {
        public:
            ProfileEditor();
            ProfileEditor(Service Srv);
            Result Store(AccountProfileBase *PBase, AccountUserData *UData);
            Result StoreWithImage(AccountProfileBase *PBase, AccountUserData *UData, u8 *JPEG, size_t JPEGSize);
            void Close();
        private:
            Service srv;
    };

    class BaasAdministrator
    {
        public:
            BaasAdministrator();
            BaasAdministrator(Service Srv);
            Result IsLinkedWithNintendoAccount(bool *out);
            Result DeleteRegistrationInfoLocally();
            Result GetAccountId(u64 *out_AId);
            Result GetNintendoAccountId(u64 *out_NAId);
            void Close();
        private:
            Service srv;
    };

    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result DeleteUser(u128 UserId);
    Result GetProfileEditor(u128 UserId, ProfileEditor *out_Editor);
    Result GetBaasAccountAdministrator(u128 UserId, BaasAdministrator *out_Admin);
}