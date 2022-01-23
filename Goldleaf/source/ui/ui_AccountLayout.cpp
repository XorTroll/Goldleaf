
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

#include <ui/ui_AccountLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    AccountLayout::AccountLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->options_menu);
        this->ReloadItems();
        this->Add(this->options_menu);
    }

    void AccountLayout::ReloadItems() {
        this->options_menu->ClearItems();
        auto rename_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(208));
        rename_itm->SetColor(g_Settings.custom_scheme.text);
        rename_itm->AddOnKey(std::bind(&AccountLayout::optsRename_DefaultKey, this));
        this->options_menu->AddItem(rename_itm);

        auto icon_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(209));
        icon_itm->SetColor(g_Settings.custom_scheme.text);
        icon_itm->AddOnKey(std::bind(&AccountLayout::optsIcon_DefaultKey, this));
        this->options_menu->AddItem(icon_itm);

        auto delete_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(210));
        delete_itm->SetColor(g_Settings.custom_scheme.text);
        delete_itm->AddOnKey(std::bind(&AccountLayout::optsDelete_DefaultKey, this));
        this->options_menu->AddItem(delete_itm);

        if(acc::IsLinked()) {
            auto services_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(336));
            services_itm->SetColor(g_Settings.custom_scheme.text);
            services_itm->AddOnKey(std::bind(&AccountLayout::optsServicesInfo_DefaultKey, this));
            this->options_menu->AddItem(services_itm);
        }
    }

    void AccountLayout::Load() {
        AccountProfileBase prof_base = {};
        const auto rc = acc::ReadSelectedUser(&prof_base, nullptr);
        if(R_FAILED(rc)) {
            HandleResult(rc, cfg::strings::Main.GetString(211));
            g_MainApplication->ReturnToMainMenu();
            return;
        }

        acc::CacheSelectedUserIcon();
        auto sd_exp = fs::GetSdCardExplorer();
        auto user_icon = acc::GetCachedUserIcon();
        const auto default_icon = sd_exp->Exists(user_icon);
        if(default_icon) {
            user_icon = "Accounts";
        }
        
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(41), user_icon, cfg::strings::Main.GetString(212) + " " + prof_base.nickname, default_icon);
        this->ReloadItems();
    }

    void AccountLayout::optsRename_DefaultKey() {
        const auto name = AskForText(cfg::strings::Main.GetString(213), "", 10);
        if(!name.empty()) {
            const auto rc = acc::EditUser([&](AccountProfileBase *prof_base, AccountUserData *_user_data) {
                strcpy(prof_base->nickname, name.c_str());
            });
            if(R_SUCCEEDED(rc)) {
                g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(212) + " " + name);
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(214) + " \'" + name + "\'.");
            }
            else {
                HandleResult(rc, cfg::strings::Main.GetString(215));
            }
        }
    }

    void AccountLayout::optsIcon_DefaultKey() {
        const auto &base_icon_path = GLEAF_PATH_USER_DATA_DIR "/" + hos::FormatHex128(acc::GetSelectedUser()) + ".jpg";
        auto sd_exp = fs::GetSdCardExplorer();
        const auto icon_path = sd_exp->MakeAbsolute(base_icon_path);
        const auto p_icon_path = sd_exp->MakeAbsolutePresentable(base_icon_path);
        g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(217) + "\n\'" + p_icon_path + "\'", { cfg::strings::Main.GetString(234) }, false, icon_path);
    }

    void AccountLayout::optsDelete_DefaultKey() {
        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(218), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(option == 0) {
            if(acc::GetUserCount() < 2) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(216), cfg::strings::Main.GetString(276), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            const auto rc = acc::DeleteUser(acc::GetSelectedUser());
            if(R_SUCCEEDED(rc)) {
                acc::ResetSelectedUser();
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(219));
                g_MainApplication->ReturnToMainMenu();
            }
            else {
                HandleResult(rc, cfg::strings::Main.GetString(220));
            }
        }
    }

    void AccountLayout::optsServicesInfo_DefaultKey() {
        const auto linked_info = acc::GetUserLinkedInfo();
        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(330), cfg::strings::Main.GetString(328) + " " + hos::FormatHex(linked_info.account_id) + "\n" + cfg::strings::Main.GetString(329) + " " + hos::FormatHex(linked_info.nintendo_account_id), { cfg::strings::Main.GetString(331), cfg::strings::Main.GetString(18) }, true);
        if(option_1 == 0) {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(332), cfg::strings::Main.GetString(333), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
                const auto rc = acc::UnlinkLocally();
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(334));
                }
                else {
                    HandleResult(rc, cfg::strings::Main.GetString(335));
                }
            }
        }
    }

}