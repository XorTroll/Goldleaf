
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

#include <acc/acc_UserSelection.hpp>
#include <acc/acc_Service.hpp>
#include <fs/fs_Common.hpp>
#include <hos/hos_Common.hpp>

namespace acc
{
    static AccountUid selected_user;

    AccountUid GetSelectedUser()
    {
        return selected_user;
    }

    bool HasUser()
    {
        return accountUidIsValid(&selected_user);
    }

    void SetSelectedUser(AccountUid uid)
    {
        memcpy(&selected_user, &uid, sizeof(uid));
    }

    void ResetSelectedUser()
    {
        selected_user = {};
    }

    bool SelectFromPreselectedUser()
    {
        AccountUid tmpuser = {};
        auto res = accountGetPreselectedUser(&tmpuser);
        if(R_SUCCEEDED(res) && accountUidIsValid(&tmpuser))
        {
            SetSelectedUser(tmpuser);
            return true;
        }
        return false;
    }

    bool SelectUser()
    {
        AccountUid user = LaunchPlayerSelect();
        if(accountUidIsValid(&user))
        {
            SetSelectedUser(user);
            return true;
        }
        return false;
    }

    AccountUid LaunchPlayerSelect()
    {
        AccountUid out_id = {};
        LibAppletArgs args;
        libappletArgsCreate(&args, 0x10000);
        u8 st_in[0xA0] = {0};
        u8 st_out[0x18] = {0};
        size_t repsz;

        auto res = libappletLaunch(AppletId_playerSelect, &args, st_in, 0xA0, st_out, 0x18, &repsz);
        if(R_SUCCEEDED(res))
        {
            u64 lres = *(u64*)st_out;
            AccountUid *uid_ptr = (AccountUid*)&st_out[8];
            if(lres == 0) memcpy(&out_id, uid_ptr, sizeof(out_id));
        }
        
        return out_id;
    }

    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb)
    {
        AccountProfile prof;
        auto rc = accountGetProfile(&prof, selected_user);
        if(R_SUCCEEDED(rc))
        {
            AccountProfileBase pbase = {};
            AccountUserData udata = {};
            rc = accountProfileGet(&prof, &udata, &pbase);
            if(R_SUCCEEDED(rc))
            {
                Service editor;
                rc = GetProfileEditor(selected_user, &editor);
                if(R_SUCCEEDED(rc))
                {
                    cb(&pbase, &udata);
                    rc = ProfileEditor_Store(&editor, pbase, udata);
                    serviceClose(&editor);
                }
            }
            accountProfileClose(&prof);
        }
        return rc;
    }

    Result EditUserIcon(u8 *jpg, size_t size)
    {
        AccountProfile prof;
        auto rc = accountGetProfile(&prof, selected_user);
        if(R_SUCCEEDED(rc))
        {
            AccountProfileBase pbase = {};
            AccountUserData udata = {};
            rc = accountProfileGet(&prof, &udata, &pbase);
            if(R_SUCCEEDED(rc))
            {
                Service editor;
                rc = GetProfileEditor(selected_user, &editor);
                if(R_SUCCEEDED(rc))
                {
                    rc = ProfileEditor_StoreWithImage(&editor, pbase, udata, jpg, size);
                    serviceClose(&editor);
                }
            }
            accountProfileClose(&prof);
        }
        return rc;
    }

    void CacheSelectedUserIcon()
    {
        AccountProfile prof;
        auto res = accountGetProfile(&prof, selected_user);
        if(res == 0)
        {
            u32 iconsize = 0;
            accountProfileGetImageSize(&prof, &iconsize);
            if(iconsize > 0)
            {
                u8 *icon = new u8[iconsize]();
                u32 tmpsz;
                res = accountProfileLoadImage(&prof, icon, iconsize, &tmpsz);
                if(res == 0)
                {
                    std::string iconpth = GetCachedUserIcon();
                    fs::DeleteFile(iconpth);
                    FILE *f = fopen(iconpth.c_str(), "wb");
                    if(f)
                    {
                        fwrite(icon, 1, iconsize, f);
                        fclose(f);
                    }
                }
                delete[] icon;
            }
            accountProfileClose(&prof);
        }
    }

    std::string GetCachedUserIcon()
    {
        return "sdmc:/" + consts::Root + "/userdata/" + hos::FormatHex128(selected_user) + ".jpg";
    }

    bool IsLinked()
    {
        bool linked = false;
        Service baas;
        auto rc = GetBaasAccountAdministrator(selected_user, &baas);
        if(R_SUCCEEDED(rc))
        {
            BaasAdministrator_IsLinkedWithNintendoAccount(&baas, &linked);
            serviceClose(&baas);
        }
        return linked;
    }

    Result UnlinkLocally()
    {
        Service baas;
        auto rc = GetBaasAccountAdministrator(selected_user, &baas);
        if(R_SUCCEEDED(rc))
        {
            bool linked = false;
            rc = BaasAdministrator_IsLinkedWithNintendoAccount(&baas, &linked);
            if(R_SUCCEEDED(rc) && linked) rc = BaasAdministrator_DeleteRegistrationInfoLocally(&baas);
            serviceClose(&baas);
        }
        return rc;
    }

    LinkedAccountInfo GetUserLinkedInfo()
    {
        LinkedAccountInfo info = {};
        Service baas;
        auto rc = GetBaasAccountAdministrator(selected_user, &baas);
        if(R_SUCCEEDED(rc))
        {
            BaasAdministrator_GetAccountId(&baas, &info.AccountId);
            BaasAdministrator_GetNintendoAccountId(&baas, &info.NintendoAccountId);
            serviceClose(&baas);
        }
        return info;
    }
}