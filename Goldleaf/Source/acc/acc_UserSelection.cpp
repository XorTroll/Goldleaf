#include <acc/acc_UserSelection.hpp>
#include <acc/acc_Service.hpp>
#include <fs/fs_Common.hpp>
#include <hos/hos_Common.hpp>

namespace acc
{
    static u128 selected_user = 0;

    u128 GetSelectedUser()
    {
        return selected_user;
    }

    void SetSelectedUser(u128 User)
    {
        selected_user = User;
    }

    bool SelectFromPreselectedUser()
    {
        u128 tmpuser = 0;
        auto res = accountGetPreselectedUser(&tmpuser);
        if(R_SUCCEEDED(res) && (tmpuser != 0))
        {
            selected_user = tmpuser;
            return true;
        }
        return false;
    }

    bool SelectUser()
    {
        u128 user = LaunchPlayerSelect();
        if(user != 0)
        {
            selected_user = user;
            return true;
        }
        return false;
    }

    u128 LaunchPlayerSelect()
    {
        u128 out_id = 0;
        LibAppletArgs args;
        libappletArgsCreate(&args, 0x10000);
        u8 st_in[0xA0] = {0};
        u8 st_out[0x18] = {0};
        size_t repsz;

        auto res = libappletLaunch(AppletId_playerSelect, &args, st_in, 0xA0, st_out, 0x18, &repsz);
        if(R_SUCCEEDED(res))
        {
            u64 lres = *(u64*)st_out;
            u128 uid = *(u128*)&st_out[8];
            if(lres == 0) out_id = uid;
        }
        
        return out_id;
    }

    Result EditUser(std::function<void(AccountProfileBase*, AccountUserData*)> cb)
    {
        AccountProfile prof;
        auto res = accountGetProfile(&prof, selected_user);
        if(res == 0)
        {
            AccountProfileBase pbase = {};
            AccountUserData udata = {};
            res = accountProfileGet(&prof, &udata, &pbase);
            if(res == 0)
            {
                ProfileEditor editor;
                res = GetProfileEditor(selected_user, &editor);
                if(res == 0)
                {
                    cb(&pbase, &udata);
                    editor.Store(&pbase, &udata);
                }
                editor.Close();
            }
            accountProfileClose(&prof);
        }
        return res;
    }

    Result EditUserIcon(u8 *JPEG, size_t JPEG_size)
    {
        AccountProfile prof;
        auto res = accountGetProfile(&prof, selected_user);
        if(res == 0)
        {
            AccountProfileBase pbase = {};
            AccountUserData udata = {};
            res = accountProfileGet(&prof, &udata, &pbase);
            if(res == 0)
            {
                ProfileEditor editor;
                res = GetProfileEditor(selected_user, &editor);
                if(res == 0) editor.StoreWithImage(&pbase, &udata, JPEG, JPEG_size);
                editor.Close();
            }
            accountProfileClose(&prof);
        }
        return res;
    }

    void CacheSelectedUserIcon()
    {
        AccountProfile prof;
        auto res = accountGetProfile(&prof, selected_user);
        if(res == 0)
        {
            size_t iconsize = 0;
            accountProfileGetImageSize(&prof, &iconsize);
            if(iconsize > 0)
            {
                u8 *icon = new u8[iconsize]();
                size_t tmpsz;
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
        return "sdmc:/" + GoldleafDir + "/userdata/" + hos::FormatHex128(selected_user) + ".jpg";
    }
}