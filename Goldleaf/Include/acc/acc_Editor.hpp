
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

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

    Result Initialize();
    void Finalize();
    bool HasInitialized();

    Result DeleteUser(u128 UserId);
    Result GetProfileEditor(u128 UserId, ProfileEditor *out_Editor);
}