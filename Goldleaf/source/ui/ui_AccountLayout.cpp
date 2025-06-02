
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

    namespace {

        Result DeleteUser() {
            const auto rc = accountextDeleteUser(acc::GetSelectedUser());
            if(R_SUCCEEDED(rc)) {
                acc::ResetSelectedUser();
                g_MainApplication->ShowNotification(cfg::Strings.GetString(219));
                g_MainApplication->ReturnToParentLayout();
            }
            return rc;
        }

    }

    void AccountLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    void AccountLayout::ReloadItems() {
        this->options_menu->ClearItems();
        auto rename_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(208));
        rename_itm->SetColor(g_Settings.GetColorScheme().text);
        rename_itm->AddOnKey(std::bind(&AccountLayout::optsRename_DefaultKey, this));
        this->options_menu->AddItem(rename_itm);

        auto icon_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(209));
        icon_itm->SetColor(g_Settings.GetColorScheme().text);
        icon_itm->AddOnKey(std::bind(&AccountLayout::optsIcon_DefaultKey, this));
        this->options_menu->AddItem(icon_itm);

        auto delete_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(210));
        delete_itm->SetColor(g_Settings.GetColorScheme().text);
        delete_itm->AddOnKey(std::bind(&AccountLayout::optsDelete_DefaultKey, this));
        this->options_menu->AddItem(delete_itm);

        if(acc::IsLinked()) {
            auto services_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(336));
            services_itm->SetColor(g_Settings.GetColorScheme().text);
            services_itm->AddOnKey(std::bind(&AccountLayout::optsServicesInfo_DefaultKey, this));
            this->options_menu->AddItem(services_itm);
        }
    }

    void AccountLayout::optsRename_DefaultKey() {
        const auto name = ShowKeyboard(cfg::Strings.GetString(213), this->cur_prof_base.nickname, 10, sizeof(this->cur_prof_base.nickname) - 1);
        if(!name.empty()) {
            strcpy(this->cur_prof_base.nickname, name.c_str());
            const auto rc = acc::EditUser([&](AccountProfileBase *prof_base, AccountUserData *_user_data) {
                memcpy(prof_base, &this->cur_prof_base, sizeof(this->cur_prof_base));
            });
            if(R_SUCCEEDED(rc)) {
                g_MainApplication->LoadMenuHead(cfg::Strings.GetString(212) + " " + name);
                g_MainApplication->ShowNotification(cfg::Strings.GetString(214) + " \'" + name + "\'.");
            }
            else {
                HandleResult(rc, cfg::Strings.GetString(215));
            }
        }
    }

    void AccountLayout::optsIcon_DefaultKey() {
        const auto base_icon_path = GLEAF_PATH_USER_DATA_DIR "/" + util::FormatHex128(acc::GetSelectedUser()) + ".jpg";
        auto sd_exp = fs::GetSdCardExplorer();
        const auto icon_path = sd_exp->MakeAbsolute(base_icon_path);
        const auto p_icon_path = sd_exp->MakeAbsolutePresentable(base_icon_path);
        g_MainApplication->DisplayDialog(cfg::Strings.GetString(216), cfg::Strings.GetString(217) + "\n\'" + p_icon_path + "\'", { cfg::Strings.GetString(234) }, false, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(icon_path)));
    }

    void AccountLayout::optsDelete_DefaultKey() {
        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(216), cfg::Strings.GetString(218), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
        if(option == 0) {
            if(acc::GetUserCount() < 2) {
                g_MainApplication->DisplayDialog(cfg::Strings.GetString(216), cfg::Strings.GetString(276), { cfg::Strings.GetString(234) }, true);
                return;
            }

            auto rc = DeleteUser();
            if(R_FAILED(rc)) {
                if(rc == rc::account::ResultUnableToRemoveLinkedAccount) {
                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(216), cfg::Strings.GetString(480), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                    if(option_2 == 0) {
                        rc = acc::UnlinkLocally();
                        if(R_SUCCEEDED(rc)) {
                            rc = DeleteUser();
                        }
                    }
                }

                HandleResult(rc, cfg::Strings.GetString(220));
            }
        }
    }

    void AccountLayout::optsServicesInfo_DefaultKey() {
        const auto linked_info = acc::GetUserLinkedInfo();
        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(330), cfg::Strings.GetString(328) + " " + util::FormatHex(linked_info.account_id) + "\n" + cfg::Strings.GetString(329) + " " + util::FormatHex(linked_info.nintendo_account_id), { cfg::Strings.GetString(331), cfg::Strings.GetString(18) }, true);
        if(option_1 == 0) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(332), cfg::Strings.GetString(333), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                const auto rc = acc::UnlinkLocally();
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(334));
                }
                else {
                    HandleResult(rc, cfg::Strings.GetString(335));
                }
            }
        }
    }

    AccountLayout::AccountLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->options_menu);
        this->ReloadItems();
        this->Add(this->options_menu);

        this->SetOnInput(std::bind(&AccountLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void AccountLayout::Load() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(41), CommonIconKind::Accounts, cfg::Strings.GetString(42));
        const auto rc = acc::ReadSelectedUser(&this->cur_prof_base, nullptr);
        if(R_FAILED(rc)) {
            HandleResult(rc, cfg::Strings.GetString(211));
            g_MainApplication->ReturnToParentLayout();
            return;
        }

        acc::ExportSelectedUserIcon();
        auto sd_exp = fs::GetSdCardExplorer();
        auto user_icon = acc::GetExportedUserIcon();
        const auto menu_head = cfg::Strings.GetString(212) + " " + this->cur_prof_base.nickname;
        if(sd_exp->Exists(user_icon)) {
            g_MainApplication->LoadFileMenuData(false, cfg::Strings.GetString(41), user_icon, menu_head);
        }
        else {
            g_MainApplication->LoadCommonIconMenuData(false, cfg::Strings.GetString(41), CommonIconKind::Accounts, menu_head);
        }
        this->ReloadItems();
    }

}
