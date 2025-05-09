
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

#include <ui/ui_WebBrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline Result LaunchWebAppletImpl(const std::string &page) {
            WebCommonConfig web = {};
            GLEAF_RC_TRY(webPageCreate(&web, page.c_str()));
            GLEAF_RC_TRY(webConfigSetWhitelist(&web, ".*"));
            if(acc::HasSelectedUser()) {
                GLEAF_RC_TRY(webConfigSetUid(&web, acc::GetSelectedUser()));
            }

            return webConfigShow(&web, nullptr);
        }

    }

    void WebBrowserLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    WebBrowserLayout::WebBrowserLayout() : pu::ui::Layout() {
        this->opts_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->opts_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        this->Add(this->opts_menu);

        this->SetOnInput(std::bind(&WebBrowserLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void WebBrowserLayout::Refresh() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(36), CommonIconKind::Browser, cfg::Strings.GetString(14));

        this->opts_menu->ClearItems();
    
        auto input_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(378));
        input_itm->SetColor(g_Settings.GetColorScheme().text);
        input_itm->AddOnKey(std::bind(&WebBrowserLayout::input_DefaultKey, this));
        this->opts_menu->AddItem(input_itm);

        for(const auto &bmk: g_Settings.bookmarks) {
            auto bmk_itm = pu::ui::elm::MenuItem::New(bmk.name);
            bmk_itm->SetColor(g_Settings.GetColorScheme().text);
            bmk_itm->SetIcon(GetCommonIcon(CommonIconKind::Browser));
            bmk_itm->AddOnKey(std::bind(&WebBrowserLayout::bookmark_DefaultKey, this, bmk));
            this->opts_menu->AddItem(bmk_itm);
        }
    }

    void WebBrowserLayout::input_DefaultKey() {
        const auto url = ShowKeyboard(cfg::Strings.GetString(38), "https://");
        if(!url.empty()) {
            LaunchWebAppletImpl(url);

            const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(379), cfg::Strings.GetString(380), { cfg::Strings.GetString(111), cfg::Strings.GetString(112) }, true);
            if(option == 0) {
                const auto name = ShowKeyboard(cfg::Strings.GetString(381));
                if(!name.empty()) {
                    const cfg::WebBookmark bmk = { name, url };
                    g_Settings.bookmarks.push_back(bmk);
                    g_Settings.Save();
                    this->Refresh();
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(382));
                }
            }
        }
    }
    
    void WebBrowserLayout::bookmark_DefaultKey(cfg::WebBookmark &bmk) {
        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(383), cfg::Strings.GetString(384), { cfg::Strings.GetString(385), cfg::Strings.GetString(386), cfg::Strings.GetString(245), cfg::Strings.GetString(18) }, true);
        switch(option_1) {
            case 0: {
                const auto rc = LaunchWebAppletImpl(bmk.url);
                if(R_FAILED(rc)) {
                    HandleResult(rc, cfg::Strings.GetString(40));
                }
                break;
            }
            case 1: {
                const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(387), cfg::Strings.GetString(388), { cfg::Strings.GetString(389), cfg::Strings.GetString(390), cfg::Strings.GetString(18) }, true);
                switch(option_2) {
                    case 0: {
                        const auto name = ShowKeyboard(cfg::Strings.GetString(391));
                        if(!name.empty()) {
                            for(auto &saved_bmk: g_Settings.bookmarks) {
                                if(saved_bmk.name == bmk.name) {
                                    saved_bmk.name = name;
                                    g_Settings.Save();
                                    this->Refresh();
                                    g_MainApplication->ShowNotification(cfg::Strings.GetString(393));
                                }
                            }
                        }
                        break;
                    }
                    case 1: {
                        const auto url = ShowKeyboard(cfg::Strings.GetString(392), "https://");
                        if(!url.empty()) {
                            for(auto &saved_bmk: g_Settings.bookmarks) {
                                if(saved_bmk.name == bmk.name) {
                                    saved_bmk.url = url;
                                    g_Settings.Save();
                                    this->Refresh();
                                    g_MainApplication->ShowNotification(cfg::Strings.GetString(394));
                                }
                            }
                            
                        }
                        break;
                    }
                }
                break;
            }
            case 2: {
                const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(395), cfg::Strings.GetString(396), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                switch(option_2) {
                    case 0: {
                        for(u32 i = 0; i < g_Settings.bookmarks.size(); i++) {
                            const auto &saved_bmk = g_Settings.bookmarks[i];
                            if(bmk.name == saved_bmk.name) {
                                g_Settings.bookmarks.erase(g_Settings.bookmarks.begin() + i);
                                g_Settings.Save();
                                this->opts_menu->SetSelectedIndex(this->opts_menu->GetSelectedIndex() - 1);
                                this->Refresh();
                                g_MainApplication->ShowNotification(cfg::Strings.GetString(397));
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

}
