
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

#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    MainMenuLayout::MainMenuLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->options_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->options_menu);
        this->explore_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(277));
        this->explore_menu_item->SetIcon(g_Settings.PathForResource("/Common/SdCard.png"));
        this->explore_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->explore_menu_item->AddOnClick(std::bind(&MainMenuLayout::ExploreMenu_OnClick, this));
        this->cnt_manager_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(3));
        this->cnt_manager_menu_item->SetIcon(g_Settings.PathForResource("/Common/Storage.png"));
        this->cnt_manager_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->cnt_manager_menu_item->AddOnClick(std::bind(&MainMenuLayout::ContentManager_OnClick, this));
        this->web_browser_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(5));
        this->web_browser_menu_item->SetIcon(g_Settings.PathForResource("/Common/Browser.png"));
        this->web_browser_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->web_browser_menu_item->AddOnClick(std::bind(&MainMenuLayout::WebBrowser_OnClick, this));
        this->account_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(6));
        this->account_menu_item->SetIcon(g_Settings.PathForResource("/Common/Accounts.png"));
        this->account_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->account_menu_item->AddOnClick(std::bind(&MainMenuLayout::Account_OnClick, this));
        this->amiibo_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(283));
        this->amiibo_menu_item->SetIcon(g_Settings.PathForResource("/Common/Amiibo.png"));
        this->amiibo_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->amiibo_menu_item->AddOnClick(std::bind(&MainMenuLayout::Amiibo_OnClick, this));
        this->settings_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(375));
        this->settings_menu_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->settings_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->settings_menu_item->AddOnClick(std::bind(&MainMenuLayout::Settings_OnClick, this));
        this->update_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(284));
        this->update_menu_item->SetIcon(g_Settings.PathForResource("/Common/Update.png"));
        this->update_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->update_menu_item->AddOnClick(std::bind(&MainMenuLayout::Update_OnClick, this));
        this->about_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(8));
        this->about_menu_item->SetIcon(g_Settings.PathForResource("/Common/Info.png"));
        this->about_menu_item->SetColor(g_Settings.custom_scheme.Text);
        this->about_menu_item->AddOnClick(std::bind(&MainMenuLayout::About_OnClick, this));
        this->options_menu->AddItem(this->explore_menu_item);
        this->options_menu->AddItem(this->cnt_manager_menu_item);
        this->options_menu->AddItem(this->web_browser_menu_item);
        this->options_menu->AddItem(this->account_menu_item);
        this->options_menu->AddItem(this->amiibo_menu_item);
        this->options_menu->AddItem(this->settings_menu_item);
        this->options_menu->AddItem(this->update_menu_item);
        this->options_menu->AddItem(this->about_menu_item);
        this->Add(this->options_menu);
    }

    void MainMenuLayout::ExploreMenu_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(277), "Storage", cfg::strings::Main.GetString(278));
        g_MainApplication->GetExploreMenuLayout()->UpdateMenu();
        g_MainApplication->LoadLayout(g_MainApplication->GetExploreMenuLayout());
    }

    void MainMenuLayout::ContentManager_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(32), "Storage", cfg::strings::Main.GetString(33));
        EnsureDirectories();
        g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
    }

    void MainMenuLayout::WebBrowser_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(36), "Browser", cfg::strings::Main.GetString(14));
        g_MainApplication->GetWebBrowserLayout()->Refresh();
        g_MainApplication->LoadLayout(g_MainApplication->GetWebBrowserLayout());
    }

    void MainMenuLayout::Account_OnClick() {
        if(!acc::HasSelectedUser()) {
            const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(348), cfg::strings::Main.GetString(349), {cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18)}, true);
            if(option == 0) {
                if(acc::SelectUser()) {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(324));
                }
                else {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(350));
                    return;
                }
            }
            else {
                return;
            }
        }
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(41), "Accounts", cfg::strings::Main.GetString(42));
        g_MainApplication->GetAccountLayout()->Load();
        g_MainApplication->LoadLayout(g_MainApplication->GetAccountLayout());
    }

    void MainMenuLayout::Amiibo_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(283), "Amiibo", cfg::strings::Main.GetString(301));
        g_MainApplication->LoadLayout(g_MainApplication->GetAmiiboDumpLayout());
        g_MainApplication->GetAmiiboDumpLayout()->StartDump();
        g_MainApplication->ReturnToMainMenu();
    }

    void MainMenuLayout::Settings_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(375), "Settings", cfg::strings::Main.GetString(376));
        g_MainApplication->LoadLayout(g_MainApplication->GetSettingsLayout());
    }

    void MainMenuLayout::Update_OnClick() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(284), "Update", cfg::strings::Main.GetString(302));
        g_MainApplication->LoadLayout(g_MainApplication->GetUpdateLayout());
        g_MainApplication->GetUpdateLayout()->StartUpdateSearch();
    }

    void MainMenuLayout::About_OnClick() {
        auto exec_mode_str = cfg::strings::Main.GetString((GetExecutableMode() == ExecutableMode::NRO) ? 288 : 289);
        auto launch_mode_str = cfg::strings::Main.GetString((GetLaunchMode() == LaunchMode::Applet) ? 290 : 291);
        g_MainApplication->LoadMenuData("Goldleaf v" GOLDLEAF_VERSION, "Info", exec_mode_str + ", " + launch_mode_str);
        g_MainApplication->LoadLayout(g_MainApplication->GetAboutLayout());
    }

}