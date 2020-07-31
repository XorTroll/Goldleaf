
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <ui/ui_StorageContentsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    StorageContentsLayout::StorageContentsLayout()
    {
        this->contentsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->contentsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->contentsMenu);
        this->noContentsText = pu::ui::elm::TextBlock::New(0, 0, cfg::strings::Main.GetString(188));
        this->noContentsText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->noContentsText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->noContentsText->SetColor(global_settings.custom_scheme.Text);
        this->noContentsText->SetVisible(false);
        this->Add(this->noContentsText);
        this->Add(this->contentsMenu);
    }

    void StorageContentsLayout::contents_Click()
    {
        auto &selcnt = this->contents[this->contentsMenu->GetSelectedIndex()];
        global_app->GetContentInformationLayout()->LoadContent(selcnt);
        global_app->LoadLayout(global_app->GetContentInformationLayout());
    }

    void StorageContentsLayout::LoadFromStorage(Storage Location)
    {
        if(!this->contents.empty())
        {
            this->contentsMenu->ClearItems();
            this->contents.clear();
        }
        auto cnts = hos::SearchTitles(ncm::ContentMetaType::Any, Location);
        for(auto &cnt: cnts)
        {
            bool ok = true;
            for(auto content: this->contents)
            {
                auto curbaseid = hos::GetBaseApplicationId(cnt.ApplicationId, cnt.Type);
                auto cntbaseid = hos::GetBaseApplicationId(content.ApplicationId, content.Type);

                if(curbaseid == cntbaseid)
                {
                    ok = false;
                    break;
                }
            }
            if(ok) this->contents.push_back(cnt);
        }

        const auto empty = this->contents.empty();
        this->noContentsText->SetVisible(empty);
        this->contentsMenu->SetVisible(!empty);
        if(!empty)
        {
            this->contentsMenu->SetCooldownEnabled(true);
            this->noContentsText->SetVisible(false);
            this->contentsMenu->SetVisible(true);
            for(auto &content: this->contents)
            {
                auto nacp = content.TryGetNACP();
                String name = hos::FormatApplicationId(content.ApplicationId);
                if(nacp != nullptr)
                {
                    name = hos::GetNACPName(nacp);
                    delete nacp;
                }
                auto itm = pu::ui::elm::MenuItem::New(name);
                itm->SetColor(global_settings.custom_scheme.Text);
                if(content.DumpControlData()) itm->SetIcon(hos::GetExportedIconPath(content.ApplicationId));
                itm->AddOnClick(std::bind(&StorageContentsLayout::contents_Click, this));
                this->contentsMenu->AddItem(itm);
            }
            this->contentsMenu->SetSelectedIndex(0);
        }
        global_app->LoadMenuHead(cfg::strings::Main.GetString(189));
    }

    std::vector<hos::Title> StorageContentsLayout::GetContents()
    {
        return this->contents;
    }
}