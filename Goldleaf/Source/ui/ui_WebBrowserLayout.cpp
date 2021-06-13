
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

#include <ui/ui_WebBrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline void LaunchWebAppletImpl(const std::string &page) {
            WebCommonConfig web = {};
            webPageCreate(&web, page.c_str());
            webConfigSetWhitelist(&web, ".*");
            if(acc::HasSelectedUser()) {
                webConfigSetUid(&web, acc::GetSelectedUser());
            }
            webConfigShow(&web, nullptr);
        }

    }

    WebBrowserLayout::WebBrowserLayout() : pu::ui::Layout() {
        this->opts_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, (560 / g_Settings.menu_item_size));
        this->opts_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->opts_menu);
        this->Add(this->opts_menu);
    }

    void WebBrowserLayout::Refresh() {
        this->opts_menu->ClearItems();
    
        auto input_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(378));
        input_itm->SetColor(g_Settings.custom_scheme.Text);
        input_itm->AddOnClick(std::bind(&WebBrowserLayout::input_Click, this));
        this->opts_menu->AddItem(input_itm);

        for(const auto &bmk: g_Settings.bookmarks) {
            auto bmk_itm = pu::ui::elm::MenuItem::New(bmk.name);
            bmk_itm->SetColor(g_Settings.custom_scheme.Text);
            bmk_itm->SetIcon(g_Settings.PathForResource("/Common/Browser.png"));
            bmk_itm->AddOnClick(std::bind(&WebBrowserLayout::bookmark_Click, this, bmk));
            this->opts_menu->AddItem(bmk_itm);
        }
    }

    void WebBrowserLayout::input_Click() {
        auto url = AskForText(cfg::strings::Main.GetString(38), "https://");
        LaunchWebAppletImpl(url.AsUTF8());

        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(379), cfg::strings::Main.GetString(380), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112) }, true);
        if(option == 0) {
            auto name = AskForText(cfg::strings::Main.GetString(381));
            if(!name.empty()) {
                const cfg::WebBookmark bmk = { name.AsUTF8(), url.AsUTF8() };
                g_Settings.bookmarks.push_back(bmk);
                g_Settings.Save();
                this->Refresh();
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(382));
            }
        }
    }
    
    void WebBrowserLayout::bookmark_Click(cfg::WebBookmark &bmk) {
        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(383), cfg::strings::Main.GetString(384), { cfg::strings::Main.GetString(385), cfg::strings::Main.GetString(386), cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(18) }, true);
        switch(option_1) {
            case 0: {
                LaunchWebAppletImpl(bmk.url);
                break;
            }
            case 1: {
                const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(387), cfg::strings::Main.GetString(388), { cfg::strings::Main.GetString(389), cfg::strings::Main.GetString(390), cfg::strings::Main.GetString(18) }, true);
                switch(option_2) {
                    case 0: {
                        auto name = AskForText(cfg::strings::Main.GetString(391));
                        if(!name.empty()) {
                            for(auto &saved_bmk: g_Settings.bookmarks) {
                                if(saved_bmk.name == bmk.name) {
                                    saved_bmk.name = name.AsUTF8();
                                    g_Settings.Save();
                                    this->Refresh();
                                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(393));
                                }
                            }
                        }
                        break;
                    }
                    case 1: {
                        auto url = AskForText(cfg::strings::Main.GetString(392), "https://");
                        if(!url.empty()) {
                            for(auto &saved_bmk: g_Settings.bookmarks) {
                                if(saved_bmk.name == bmk.name) {
                                    saved_bmk.url = url.AsUTF8();
                                    g_Settings.Save();
                                    this->Refresh();
                                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(394));
                                }
                            }
                            
                        }
                        break;
                    }
                }
                break;
            }
            case 2: {
                const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(395), cfg::strings::Main.GetString(396), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                switch(option_2) {
                    case 0: {
                        for(u32 i = 0; i < g_Settings.bookmarks.size(); i++) {
                            const auto &saved_bmk = g_Settings.bookmarks[i];
                            if(bmk.name == saved_bmk.name) {
                                g_Settings.bookmarks.erase(g_Settings.bookmarks.begin() + i);
                                g_Settings.Save();
                                this->opts_menu->SetSelectedIndex(this->opts_menu->GetSelectedIndex() - 1);
                                this->Refresh();
                                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(397));
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