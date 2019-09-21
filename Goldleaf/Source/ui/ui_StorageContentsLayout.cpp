
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

#include <ui/ui_StorageContentsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    StorageContentsLayout::StorageContentsLayout()
    {
        this->contentsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->contentsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->contentsMenu);
        this->noContentsText = pu::ui::elm::TextBlock::New(0, 0, set::GetDictionaryEntry(188));
        this->noContentsText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->noContentsText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->noContentsText->SetColor(gsets.CustomScheme.Text);
        this->noContentsText->SetVisible(false);
        this->Add(this->noContentsText);
        this->Add(this->contentsMenu);
    }

    void StorageContentsLayout::contents_Click()
    {
        hos::Title selcnt = this->contents[this->contentsMenu->GetSelectedIndex()];
        mainapp->GetContentInformationLayout()->LoadContent(selcnt);
        mainapp->LoadLayout(mainapp->GetContentInformationLayout());
    }

    void StorageContentsLayout::LoadFromStorage(Storage Location)
    {
        if(!this->contents.empty())
        {
            this->contentsMenu->ClearItems();
            this->contents.clear();
        }
        std::vector<hos::Title> cnts = hos::SearchTitles(ncm::ContentMetaType::Any, Location);
        if(!cnts.empty()) for(u32 i = 0; i < cnts.size(); i++)
        {
            hos::Title cnt = cnts[i];
            bool ok = true;
            if(!this->contents.empty()) for(u32 j = 0; j < this->contents.size(); j++)
            {
                u64 curappid = cnt.ApplicationId;
                u64 curbaseid = hos::GetBaseApplicationId(curappid, cnt.Type);
                u64 cntappid = this->contents[j].ApplicationId;
                u64 cntbaseid = hos::GetBaseApplicationId(cntappid, this->contents[j].Type);

                if(curbaseid == cntbaseid)
                {
                    ok = false;
                    break;
                }
            }
            if(ok) this->contents.push_back(cnt);
        }
        cnts.clear();
        if(this->contents.empty())
        {
            this->noContentsText->SetVisible(true);
            this->contentsMenu->SetVisible(false);
        }
        else
        {
            this->contentsMenu->SetCooldownEnabled(true);
            this->noContentsText->SetVisible(false);
            this->contentsMenu->SetVisible(true);
            for(u32 i = 0; i < this->contents.size(); i++)
            {
                NacpStruct *nacp = this->contents[i].TryGetNACP();
                pu::String name = hos::FormatApplicationId(this->contents[i].ApplicationId);
                if(nacp != NULL)
                {
                    name = hos::GetNACPName(nacp);
                    delete nacp;
                }
                auto itm = pu::ui::elm::MenuItem::New(name);
                itm->SetColor(gsets.CustomScheme.Text);
                bool hicon = this->contents[i].DumpControlData();
                if(hicon) itm->SetIcon(hos::GetExportedIconPath(this->contents[i].ApplicationId));
                itm->AddOnClick(std::bind(&StorageContentsLayout::contents_Click, this));
                this->contentsMenu->AddItem(itm);
            }
            this->contentsMenu->SetSelectedIndex(0);
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(189));
    }

    std::vector<hos::Title> StorageContentsLayout::GetContents()
    {
        return this->contents;
    }
}