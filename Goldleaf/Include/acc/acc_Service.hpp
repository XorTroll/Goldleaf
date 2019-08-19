
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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