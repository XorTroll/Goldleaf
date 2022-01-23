
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
        this->options_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->options_menu);
        this->explore_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(277));
        this->explore_menu_item->SetIcon(g_Settings.PathForResource("/Common/SdCard.png"));
        this->explore_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->explore_menu_item->AddOnKey(std::bind(&MainMenuLayout::exploreMenu_DefaultKey, this));
        this->cnt_manager_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(3));
        this->cnt_manager_menu_item->SetIcon(g_Settings.PathForResource("/Common/Storage.png"));
        this->cnt_manager_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->cnt_manager_menu_item->AddOnKey(std::bind(&MainMenuLayout::contentManager_DefaultKey, this));
        this->web_browser_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(5));
        this->web_browser_menu_item->SetIcon(g_Settings.PathForResource("/Common/Browser.png"));
        this->web_browser_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->web_browser_menu_item->AddOnKey(std::bind(&MainMenuLayout::webBrowser_DefaultKey, this));
        this->account_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(6));
        this->account_menu_item->SetIcon(g_Settings.PathForResource("/Common/Accounts.png"));
        this->account_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->account_menu_item->AddOnKey(std::bind(&MainMenuLayout::account_DefaultKey, this));
        this->amiibo_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(283));
        this->amiibo_menu_item->SetIcon(g_Settings.PathForResource("/Common/Amiibo.png"));
        this->amiibo_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->amiibo_menu_item->AddOnKey(std::bind(&MainMenuLayout::amiibo_DefaultKey, this));
        this->settings_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(375));
        this->settings_menu_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->settings_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->settings_menu_item->AddOnKey(std::bind(&MainMenuLayout::settings_DefaultKey, this));
        this->update_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(284));
        this->update_menu_item->SetIcon(g_Settings.PathForResource("/Common/Update.png"));
        this->update_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->update_menu_item->AddOnKey(std::bind(&MainMenuLayout::update_DefaultKey, this));
        this->about_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(8));
        this->about_menu_item->SetIcon(g_Settings.PathForResource("/Common/Info.png"));
        this->about_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->about_menu_item->AddOnKey(std::bind(&MainMenuLayout::about_DefaultKey, this));
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

    void MainMenuLayout::exploreMenu_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(277), "Storage", cfg::strings::Main.GetString(278));
        g_MainApplication->GetExploreMenuLayout()->UpdateMenu();
        g_MainApplication->LoadLayout(g_MainApplication->GetExploreMenuLayout());
    }

    void MainMenuLayout::contentManager_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(32), "Storage", cfg::strings::Main.GetString(33));
        EnsureDirectories();
        g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
    }

    void MainMenuLayout::webBrowser_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(36), "Browser", cfg::strings::Main.GetString(14));
        g_MainApplication->GetWebBrowserLayout()->Refresh();
        g_MainApplication->LoadLayout(g_MainApplication->GetWebBrowserLayout());
    }

    void MainMenuLayout::account_DefaultKey() {
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

    void MainMenuLayout::amiibo_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(283), "Amiibo", cfg::strings::Main.GetString(301));
        g_MainApplication->LoadLayout(g_MainApplication->GetAmiiboDumpLayout());
        g_MainApplication->GetAmiiboDumpLayout()->StartDump();
        g_MainApplication->ReturnToMainMenu();
    }

    void MainMenuLayout::settings_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(375), "Settings", cfg::strings::Main.GetString(376));
        g_MainApplication->LoadLayout(g_MainApplication->GetSettingsLayout());
    }

    void MainMenuLayout::update_DefaultKey() {
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(284), "Update", cfg::strings::Main.GetString(302));
        g_MainApplication->LoadLayout(g_MainApplication->GetUpdateLayout());
        g_MainApplication->GetUpdateLayout()->StartUpdateSearch();
    }

    void MainMenuLayout::about_DefaultKey() {
        auto exec_mode_str = cfg::strings::Main.GetString((GetExecutableMode() == ExecutableMode::NRO) ? 288 : 289);
        auto launch_mode_str = cfg::strings::Main.GetString((GetLaunchMode() == LaunchMode::Applet) ? 290 : 291);
        g_MainApplication->LoadMenuData("Goldleaf v" GOLDLEAF_VERSION, "Info", exec_mode_str + ", " + launch_mode_str);
        g_MainApplication->LoadLayout(g_MainApplication->GetAboutLayout());
    }

}