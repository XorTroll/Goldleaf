
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <acc/acc_UserSelection.hpp>
#include <acc/acc_Service.hpp>
#include <fs/fs_FileSystem.hpp>
#include <hos/hos_Common.hpp>

namespace acc {

    namespace {

        AccountUid g_SelectedUser = {};

        inline Result ShowUserSelector(AccountUid *out_user_id) {
            const PselUserSelectionSettings selection_cfg = {};
            return pselShowUserSelector(out_user_id, &selection_cfg);
        }
    
    }

    u32 GetUserCount() {
        s32 count = 0;
        accountGetUserCount(&count);
        return static_cast<u32>(count);
    }

    AccountUid GetSelectedUser() {
        return g_SelectedUser;
    }

    bool HasSelectedUser() {
        return accountUidIsValid(&g_SelectedUser);
    }

    void SetSelectedUser(AccountUid user_id) {
        g_SelectedUser = user_id;
    }

    void ResetSelectedUser() {
        g_SelectedUser = {};
    }

    bool SelectFromPreselectedUser() {
        AccountUid pre_user_id = {};
        const auto rc = accountGetPreselectedUser(&pre_user_id);
        if(R_SUCCEEDED(rc) && accountUidIsValid(&pre_user_id)) {
            SetSelectedUser(pre_user_id);
            return true;
        }
        return false;
    }

    bool SelectUser() {
        AccountUid user_id = {};
        if(R_SUCCEEDED(ShowUserSelector(&user_id))) {
            if(accountUidIsValid(&user_id)) {
                SetSelectedUser(user_id);
                return true;
            }
        }
        return false;
    }

    Result ReadSelectedUser(AccountProfileBase *out_prof_base, AccountUserData *out_user_data) {
        AccountProfile prof;
        auto rc = accountGetProfile(&prof, g_SelectedUser);
        if(R_SUCCEEDED(rc)) {
            rc = accountProfileGet(&prof, out_user_data, out_prof_base);
            accountProfileClose(&prof);
        }
        return rc;
    }

    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb) {
        AccountProfile prof;
        auto rc = accountGetProfile(&prof, g_SelectedUser);
        if(R_SUCCEEDED(rc)) {
            AccountProfileBase prof_base = {};
            AccountUserData user_data = {};
            rc = accountProfileGet(&prof, &user_data, &prof_base);
            if(R_SUCCEEDED(rc)) {
                Service editor;
                rc = GetProfileEditor(g_SelectedUser, &editor);
                if(R_SUCCEEDED(rc)) {
                    cb(&prof_base, &user_data);
                    rc = ProfileEditor_Store(&editor, prof_base, user_data);
                    serviceClose(&editor);
                }
            }
            accountProfileClose(&prof);
        }
        return rc;
    }

    Result EditUserIcon(u8 *jpg, size_t size) {
        AccountProfile prof;
        auto rc = accountGetProfile(&prof, g_SelectedUser);
        if(R_SUCCEEDED(rc)) {
            AccountProfileBase prof_base = {};
            AccountUserData user_data = {};
            rc = accountProfileGet(&prof, &user_data, &prof_base);
            if(R_SUCCEEDED(rc)) {
                Service editor;
                rc = GetProfileEditor(g_SelectedUser, &editor);
                if(R_SUCCEEDED(rc)) {
                    rc = ProfileEditor_StoreWithImage(&editor, prof_base, user_data, jpg, size);
                    serviceClose(&editor);
                }
            }
            accountProfileClose(&prof);
        }
        return rc;
    }

    void CacheSelectedUserIcon() {
        AccountProfile prof;
        if(R_SUCCEEDED(accountGetProfile(&prof, g_SelectedUser))) {
            u32 icon_size = 0;
            accountProfileGetImageSize(&prof, &icon_size);
            if(icon_size > 0) {
                auto icon = new u8[icon_size]();
                u32 tmp_size = 0;
                if(R_SUCCEEDED(accountProfileLoadImage(&prof, icon, icon_size, &tmp_size))) {
                    const auto icon_path = GetCachedUserIcon();
                    auto sd_exp = fs::GetSdCardExplorer();
                    sd_exp->DeleteFile(icon_path);
                    sd_exp->WriteFile(icon_path, icon, icon_size);
                }
                delete[] icon;
            }
            accountProfileClose(&prof);
        }
    }

    std::string GetCachedUserIcon() {
        auto sd_exp = fs::GetSdCardExplorer();
        return sd_exp->MakeAbsolute(consts::UserData + "/" + hos::FormatHex128(g_SelectedUser) + ".jpg").AsUTF8();
    }

    bool IsLinked() {
        bool linked = false;
        Service baas;
        const auto rc = GetBaasAccountAdministrator(g_SelectedUser, &baas);
        if(R_SUCCEEDED(rc)) {
            BaasAdministrator_IsLinkedWithNintendoAccount(&baas, &linked);
            serviceClose(&baas);
        }
        return linked;
    }

    Result UnlinkLocally() {
        Service baas;
        auto rc = GetBaasAccountAdministrator(g_SelectedUser, &baas);
        if(R_SUCCEEDED(rc)) {
            bool linked = false;
            rc = BaasAdministrator_IsLinkedWithNintendoAccount(&baas, &linked);
            if(R_SUCCEEDED(rc) && linked) {
                rc = BaasAdministrator_DeleteRegistrationInfoLocally(&baas);
            }
            serviceClose(&baas);
        }
        return rc;
    }

    LinkedAccountInfo GetUserLinkedInfo() {
        LinkedAccountInfo info = {};
        Service baas;
        const auto rc = GetBaasAccountAdministrator(g_SelectedUser, &baas);
        if(R_SUCCEEDED(rc)) {
            BaasAdministrator_GetAccountId(&baas, &info.account_id);
            BaasAdministrator_GetNintendoAccountId(&baas, &info.nintendo_account_id);
            serviceClose(&baas);
        }
        return info;
    }
}