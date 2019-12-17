
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
#include <functional>
#include <Types.hpp>

namespace acc
{
    struct LinkedAccountInfo
    {
        u64 AccountId;
        u64 NintendoAccountId;  
    };

    NX_CONSTEXPR bool UidCompare(AccountUid *a, AccountUid *b)
    {
        return memcmp(a->uid, b->uid, sizeof(AccountUid)) == 0;
    }

    AccountUid GetSelectedUser();
    bool HasUser();
    void SetSelectedUser(AccountUid uid);
    void ResetSelectedUser();
    bool SelectFromPreselectedUser();
    bool SelectUser();
    AccountUid LaunchPlayerSelect();
    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb);
    Result EditUserIcon(u8 *jpg, size_t size);
    void CacheSelectedUserIcon();
    std::string GetCachedUserIcon();
    bool IsLinked();
    Result UnlinkLocally();
    LinkedAccountInfo GetUserLinkedInfo();
}