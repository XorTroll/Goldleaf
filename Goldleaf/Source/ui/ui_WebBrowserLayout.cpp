
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

#include <ui/ui_WebBrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern set::Settings global_settings;

namespace ui
{
    WebBrowserLayout::WebBrowserLayout() : pu::ui::Layout()
    {
        this->optionsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optionsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optionsMenu);
        this->Add(this->optionsMenu);
    }

    void WebBrowserLayout::Refresh()
    {
        this->optionsMenu->ClearItems();
        auto inputitm = pu::ui::elm::MenuItem::New("Enter web-page...");
        inputitm->SetColor(global_settings.custom_scheme.Text);
        inputitm->AddOnClick(std::bind(&WebBrowserLayout::input_Click, this));
        this->optionsMenu->AddItem(inputitm);
        for(auto &bmk: global_settings.bookmarks)
        {
            auto bmkitm = pu::ui::elm::MenuItem::New(bmk.name);
            bmkitm->SetColor(global_settings.custom_scheme.Text);
            bmkitm->SetIcon(global_settings.PathForResource("/Common/Browser.png"));
            bmkitm->AddOnClick(std::bind(&WebBrowserLayout::bookmark_Click, this, bmk));
            this->optionsMenu->AddItem(bmkitm);
        }
    }

    static void LaunchWeb(std::string page)
    {
        WebCommonConfig web;
        webPageCreate(&web, page.c_str());
        webConfigShow(&web, NULL);
    }

    void WebBrowserLayout::input_Click()
    {
        String out = AskForText(set::GetDictionaryEntry(38), "https://");
        LaunchWeb(out.AsUTF8());

        auto sopt = global_app->CreateShowDialog("Bookmark save", "Would you like to save this page as a bookmark?", { "Yes", "No" }, true);
        if(sopt == 0)
        {
            auto name = AskForText("Bookmark name");
            set::WebBookmark bmk = {};
            bmk.name = name.AsUTF8();
            bmk.url = out.AsUTF8();
            global_settings.bookmarks.push_back(bmk);
            global_settings.Save();
            global_app->ShowNotification("Settings were updated and saved.");
        }
    }
    
    void WebBrowserLayout::bookmark_Click(set::WebBookmark bmk)
    {
        LaunchWeb(bmk.url);
    }
}