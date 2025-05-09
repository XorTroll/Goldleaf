
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

#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    MainMenuLayout::MainMenuLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->options_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        this->explore_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(277));
        this->explore_menu_item->SetIcon(GetCommonIcon(CommonIconKind::SdCard));
        this->explore_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->explore_menu_item->AddOnKey(std::bind(&MainMenuLayout::exploreMenu_DefaultKey, this));
        this->app_list_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(488));
        this->app_list_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Game));
        this->app_list_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->app_list_menu_item->AddOnKey(std::bind(&MainMenuLayout::appList_DefaultKey, this));
        this->tickets_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(4));
        this->tickets_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Ticket));
        this->tickets_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->tickets_menu_item->AddOnKey(std::bind(&MainMenuLayout::tickets_DefaultKey, this));
        this->web_browser_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(5));
        this->web_browser_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Browser));
        this->web_browser_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->web_browser_menu_item->AddOnKey(std::bind(&MainMenuLayout::webBrowser_DefaultKey, this));
        this->account_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(6));
        this->account_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Accounts));
        this->account_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->account_menu_item->AddOnKey(std::bind(&MainMenuLayout::account_DefaultKey, this));
        this->amiibo_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(283));
        this->amiibo_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Amiibo));
        this->amiibo_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->amiibo_menu_item->AddOnKey(std::bind(&MainMenuLayout::amiibo_DefaultKey, this));
        this->settings_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(375));
        this->settings_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->settings_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->settings_menu_item->AddOnKey(std::bind(&MainMenuLayout::settings_DefaultKey, this));
        this->about_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(8));
        this->about_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Info));
        this->about_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->about_menu_item->AddOnKey(std::bind(&MainMenuLayout::about_DefaultKey, this));
        this->options_menu->AddItem(this->explore_menu_item);
        this->options_menu->AddItem(this->app_list_menu_item);
        this->options_menu->AddItem(this->tickets_menu_item);
        this->options_menu->AddItem(this->web_browser_menu_item);
        this->options_menu->AddItem(this->account_menu_item);
        this->options_menu->AddItem(this->amiibo_menu_item);
        this->options_menu->AddItem(this->settings_menu_item);
        this->options_menu->AddItem(this->about_menu_item);
        this->Add(this->options_menu);

        this->options_menu->SetMoveWaitTimeMs(100);
        this->options_menu->SetItemAlphaIncrementSteps(15);
    }

    void MainMenuLayout::exploreMenu_DefaultKey() {
        g_MainApplication->GetExploreMenuLayout()->UpdateMenu();
        g_MainApplication->ShowLayout(g_MainApplication->GetExploreMenuLayout());
    }

    void MainMenuLayout::appList_DefaultKey() {
        EnsureDirectories();
        g_MainApplication->GetApplicationListLayout()->Reload();
        g_MainApplication->ShowLayout(g_MainApplication->GetApplicationListLayout());
    }

    void MainMenuLayout::tickets_DefaultKey() {
        g_MainApplication->GetTicketsLayout()->UpdateElements();
        g_MainApplication->ShowLayout(g_MainApplication->GetTicketsLayout());
    }

    void MainMenuLayout::webBrowser_DefaultKey() {
        if(GetLaunchMode() == LaunchMode::Application) {
            g_MainApplication->GetWebBrowserLayout()->Refresh();
            g_MainApplication->ShowLayout(g_MainApplication->GetWebBrowserLayout());
        }
        else {
            g_MainApplication->ShowNotification(cfg::Strings.GetString(292));
        }
    }

    void MainMenuLayout::account_DefaultKey() {
        if(!acc::HasSelectedUser()) {
            const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(348), cfg::Strings.GetString(349), {cfg::Strings.GetString(111), cfg::Strings.GetString(18)}, true);
            if(option == 0) {
                if(acc::SelectUser()) {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(324));
                }
                else {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(350));
                    return;
                }
            }
            else {
                return;
            }
        }

        g_MainApplication->GetAccountLayout()->Load();
        g_MainApplication->ShowLayout(g_MainApplication->GetAccountLayout());
    }

    void MainMenuLayout::amiibo_DefaultKey() {
        g_MainApplication->ShowLayout(g_MainApplication->GetAmiiboDumpLayout());
        g_MainApplication->GetAmiiboDumpLayout()->StartDump();
    }

    void MainMenuLayout::settings_DefaultKey() {
        g_MainApplication->ShowLayout(g_MainApplication->GetSettingsLayout());
        g_MainApplication->GetSettingsLayout()->Load();
    }

    void MainMenuLayout::about_DefaultKey() {
        g_MainApplication->ShowLayout(g_MainApplication->GetAboutLayout());
        g_MainApplication->GetAboutLayout()->Load();
    }

}
