
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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

    u128 GetSelectedUser();
    void SetSelectedUser(u128 User);
    bool SelectFromPreselectedUser();
    bool SelectUser();
    u128 LaunchPlayerSelect();
    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb);
    Result EditUserIcon(u8 *JPEG, size_t JPEG_size);
    void CacheSelectedUserIcon();
    std::string GetCachedUserIcon();
    bool IsLinked();
    Result UnlinkLocally();
    LinkedAccountInfo GetUserLinkedInfo();
}