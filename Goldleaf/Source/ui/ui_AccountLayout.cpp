
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

#include <ui/ui_AccountLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    AccountLayout::AccountLayout() : pu::ui::Layout()
    {
        this->optsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optsMenu);
        this->ReloadItems();
        this->Add(this->optsMenu);
    }

    void AccountLayout::ReloadItems()
    {
        this->optsMenu->ClearItems();
        auto itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(208));
        itm->SetColor(global_settings.custom_scheme.Text);
        itm->AddOnClick(std::bind(&AccountLayout::optsRename_Click, this));
        this->optsMenu->AddItem(itm);
        auto itm2 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(209));
        itm2->SetColor(global_settings.custom_scheme.Text);
        itm2->AddOnClick(std::bind(&AccountLayout::optsIcon_Click, this));
        this->optsMenu->AddItem(itm2);
        auto itm3 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(210));
        itm3->SetColor(global_settings.custom_scheme.Text);
        itm3->AddOnClick(std::bind(&AccountLayout::optsDelete_Click, this));
        this->optsMenu->AddItem(itm3);
        if(acc::IsLinked())
        {
            auto itm4 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(336));
            itm4->SetColor(global_settings.custom_scheme.Text);
            itm4->AddOnClick(std::bind(&AccountLayout::optsServicesInfo_Click, this));
            this->optsMenu->AddItem(itm4);
        }
    }

    void AccountLayout::Load()
    {
        this->uid = acc::GetSelectedUser();

        AccountProfile prof;
        auto rc = accountGetProfile(&prof, this->uid);
        if(R_FAILED(rc))
        {
            HandleResult(rc, cfg::strings::Main.GetString(211));
            global_app->ReturnToMainMenu();
            return;
        }

        AccountProfileBase pbase;
        AccountUserData udata;

        rc = accountProfileGet(&prof, &udata, &pbase);
        if(R_FAILED(rc))
        {
            HandleResult(rc, cfg::strings::Main.GetString(211));
            global_app->ReturnToMainMenu();
            return;
        }

        accountProfileClose(&prof);

        acc::CacheSelectedUserIcon();
        bool deficon = false;
        auto usericon = acc::GetCachedUserIcon();
        if(fs::Exists(usericon))
        {
            deficon = true;
            usericon = "Accounts";
        }
        
        global_app->LoadMenuData(cfg::strings::Main.GetString(41), usericon, cfg::strings::Main.GetString(212) + " " + String(pbase.nickname), deficon);
        this->ReloadItems();
    }

    void AccountLayout::optsRename_Click()
    {
        String name = AskForText(cfg::strings::Main.GetString(213), "", 10);
        if(!name.empty())
        {
            auto rc = acc::EditUser([&](AccountProfileBase *pbase, AccountUserData *udata)
            {
                strcpy(pbase->nickname, name.AsUTF8().c_str());
            });
            if(R_SUCCEEDED(rc))
            {
                global_app->LoadMenuHead(cfg::strings::Main.GetString(212) + " " + name);
                global_app->ShowNotification(cfg::strings::Main.GetString(214) + " \'" + name + "\'.");
            }
            else HandleResult(rc, cfg::strings::Main.GetString(215));
        }
    }

    void AccountLayout::optsIcon_Click()
    {
        std::string iconpth = "/" + consts::Root + "/userdata/" + hos::FormatHex128(this->uid) + ".jpg";
        global_app->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(217) + "\n\'SdCard:" + iconpth + "\'", { cfg::strings::Main.GetString(234) }, false, "sdmc:" + iconpth);
    }

    void AccountLayout::optsDelete_Click()
    {
        int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(218), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(sopt == 0)
        {
            s32 ucount = 0;
            accountGetUserCount(&ucount);
            if(ucount < 2)
            {
                global_app->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(276), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            auto rc = acc::DeleteUser(this->uid);
            if(R_SUCCEEDED(rc))
            {
                global_app->ShowNotification(cfg::strings::Main.GetString(219));
                global_app->ReturnToMainMenu();

                acc::ResetSelectedUser();
            }
            else HandleResult(rc, cfg::strings::Main.GetString(220));
        }
    }

    void AccountLayout::optsServicesInfo_Click()
    {
        auto linkedinfo = acc::GetUserLinkedInfo();
        String str = cfg::strings::Main.GetString(328) + " " + hos::FormatHex(linkedinfo.AccountId);
        str += "\n" + cfg::strings::Main.GetString(329) + " " + hos::FormatHex(linkedinfo.NintendoAccountId);
        auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(330), str, { cfg::strings::Main.GetString(331), cfg::strings::Main.GetString(18) }, true);
        if(sopt != 0) return;
        sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(332), cfg::strings::Main.GetString(333), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(sopt == 0)
        {
            auto res = acc::UnlinkLocally();
            if(res == 0) global_app->ShowNotification(cfg::strings::Main.GetString(334));
            else HandleResult(res, cfg::strings::Main.GetString(335));
        }
    }
}