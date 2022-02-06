
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

#pragma once
#include <base_Common.hpp>

namespace acc {

    struct LinkedAccountInfo {
        u64 account_id;
        u64 nintendo_account_id;  
    };

    inline constexpr bool UidCompare(const AccountUid *user_id_a, const AccountUid *user_id_b) {
        return memcmp(user_id_a->uid, user_id_b->uid, sizeof(AccountUid)) == 0;
    }

    u32 GetUserCount();
    AccountUid GetSelectedUser();
    bool HasSelectedUser();
    void SetSelectedUser(const AccountUid user_id);
    void ResetSelectedUser();
    bool SelectFromPreselectedUser();
    bool SelectUser();
    Result ReadSelectedUser(AccountProfileBase *out_prof_base, AccountUserData *out_user_data);
    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb);
    Result EditUserIcon(const u8 *jpg, const size_t size);
    void CacheSelectedUserIcon();
    std::string GetCachedUserIcon();
    bool IsLinked();
    Result UnlinkLocally();
    LinkedAccountInfo GetUserLinkedInfo();

}