
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

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    AccountLayout::AccountLayout() : pu::ui::Layout()
    {
        this->optsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->optsMenu);
        this->ReloadItems();
        this->Add(this->optsMenu);
    }

    void AccountLayout::ReloadItems()
    {
        this->optsMenu->ClearItems();
        auto itm = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(208));
        itm->SetColor(gsets.CustomScheme.Text);
        itm->AddOnClick(std::bind(&AccountLayout::optsRename_Click, this));
        this->optsMenu->AddItem(itm);
        auto itm2 = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(209));
        itm2->SetColor(gsets.CustomScheme.Text);
        itm2->AddOnClick(std::bind(&AccountLayout::optsIcon_Click, this));
        this->optsMenu->AddItem(itm2);
        auto itm3 = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(210));
        itm3->SetColor(gsets.CustomScheme.Text);
        itm3->AddOnClick(std::bind(&AccountLayout::optsDelete_Click, this));
        this->optsMenu->AddItem(itm3);
        if(acc::IsLinked())
        {
            auto itm4 = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(336));
            itm4->SetColor(gsets.CustomScheme.Text);
            itm4->AddOnClick(std::bind(&AccountLayout::optsServicesInfo_Click, this));
            this->optsMenu->AddItem(itm4);
        }
    }

    void AccountLayout::Load()
    {
        this->uid = acc::GetSelectedUser();

        AccountProfile prof;
        auto rc = accountGetProfile(&prof, this->uid);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }

        AccountProfileBase pbase;
        AccountUserData udata;

        rc = accountProfileGet(&prof, &udata, &pbase);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
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
        
        mainapp->LoadMenuData(set::GetDictionaryEntry(41), usericon, set::GetDictionaryEntry(212) + " " + pu::String(pbase.username), deficon);
        this->ReloadItems();
    }

    void AccountLayout::optsRename_Click()
    {
        pu::String name = AskForText(set::GetDictionaryEntry(213), "", 10);
        if(!name.empty())
        {
            auto rc = acc::EditUser([&](AccountProfileBase *pbase, AccountUserData *udata)
            {
                strcpy(pbase->username, name.AsUTF8().c_str());
            });
            if(rc == 0)
            {
                mainapp->LoadMenuHead(set::GetDictionaryEntry(212) + " " + name);
                mainapp->ShowNotification(set::GetDictionaryEntry(214) + " \'" + name + "\'.");
            }
            else HandleResult(rc, set::GetDictionaryEntry(215));
        }
    }

    void AccountLayout::optsIcon_Click()
    {
        std::string iconpth = "/" + GoldleafDir + "/userdata/" + hos::FormatHex128(this->uid) + ".jpg";
        mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(217) + "\n\'SdCard:" + iconpth + "\'", { set::GetDictionaryEntry(234) }, false, "sdmc:" + iconpth);
    }

    void AccountLayout::optsDelete_Click()
    {
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(218), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt == 0)
        {
            s32 ucount = 0;
            accountGetUserCount(&ucount);
            if(ucount < 2)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(276), { set::GetDictionaryEntry(234) }, true);
                return;
            }
            auto rc = acc::DeleteUser(this->uid);
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(219));
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
            else HandleResult(rc, set::GetDictionaryEntry(220));
        }
    }

    void AccountLayout::optsServicesInfo_Click()
    {
        auto linkedinfo = acc::GetUserLinkedInfo();
        pu::String str = set::GetDictionaryEntry(328) + " " + hos::FormatHex(linkedinfo.AccountId);
        str += "\n" + set::GetDictionaryEntry(329) + " " + hos::FormatHex(linkedinfo.NintendoAccountId);
        auto sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(330), str, { set::GetDictionaryEntry(331), set::GetDictionaryEntry(18) }, true);
        if(sopt != 0) return;
        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(332), set::GetDictionaryEntry(333), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt == 0)
        {
            auto res = acc::UnlinkLocally();
            if(res == 0) mainapp->ShowNotification(set::GetDictionaryEntry(334));
            else HandleResult(res, set::GetDictionaryEntry(335));
        }
    }
}