
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        bool SortContentsImpl(hos::Title &cnt_a, hos::Title &cnt_b) {
            std::string cnt_a_name = "";
            if(cnt_a.TryGetNacp()) {
                cnt_a_name = hos::FindNacpName(cnt_a.nacp);
            }

            std::string cnt_b_name = "";
            if(cnt_b.TryGetNacp()) {
                cnt_b_name = hos::FindNacpName(cnt_b.nacp);
            }

            if(cnt_a_name.empty() && !cnt_b_name.empty()) {
                return true;
            }
            if(!cnt_a_name.empty() && cnt_b_name.empty()) {
                return false;
            }
            if(cnt_a_name.empty() && cnt_b_name.empty()) {
                return cnt_a.app_id < cnt_b.app_id;
            }

            return cnt_a_name[0] < cnt_b_name[0];
        }

    }

    StorageContentsLayout::StorageContentsLayout() {
        this->contents_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->contents_menu);
        this->no_contents_text = pu::ui::elm::TextBlock::New(0, 0, cfg::Strings.GetString(188));
        this->no_contents_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->no_contents_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->no_contents_text->SetColor(g_Settings.custom_scheme.text);
        this->no_contents_text->SetVisible(false);
        this->Add(this->no_contents_text);
        this->Add(this->contents_menu);
    }

    void StorageContentsLayout::contents_DefaultKey() {
        auto &selected_cnt = this->contents.at(this->contents_menu->GetSelectedIndex());
        g_MainApplication->GetContentInformationLayout()->LoadContent(selected_cnt);
        g_MainApplication->LoadLayout(g_MainApplication->GetContentInformationLayout());
    }

    void StorageContentsLayout::LoadFromStorage(const NcmStorageId storage_id) {
        if(!this->contents.empty()) {
            this->contents_menu->ClearItems();
            this->contents.clear();
        }

        const auto &cnts = hos::SearchTitles(storage_id, NcmContentMetaType_Unknown);
        for(const auto &cnt: cnts) {
            bool ok = true;
            for(const auto &cur_cnt: this->contents) {
                const auto cnt_base_app_id = hos::GetBaseApplicationId(cnt.app_id, cnt.type);
                const auto cur_cnt_base_app_id = hos::GetBaseApplicationId(cur_cnt.app_id, cur_cnt.type);

                if(cnt_base_app_id == cur_cnt_base_app_id) {
                    ok = false;
                    break;
                }
            }
            if(ok) {
                this->contents.push_back(cnt);
            }
        }

        const auto is_empty = this->contents.empty();
        this->no_contents_text->SetVisible(is_empty);
        this->contents_menu->SetVisible(!is_empty);
        if(!is_empty) {
            this->contents_menu->SetCooldownEnabled(true);
            std::sort(this->contents.begin(), this->contents.end(), SortContentsImpl);
            for(auto &cur_cnt: this->contents) {
                auto name = hos::FormatApplicationId(cur_cnt.app_id);

                if(cur_cnt.TryGetNacp()) {
                    name = hos::FindNacpName(cur_cnt.nacp);
                }
                auto itm = pu::ui::elm::MenuItem::New(name);
                itm->SetColor(g_Settings.custom_scheme.text);
                if(cur_cnt.DumpControlData()) {
                    itm->SetIcon(hos::GetExportedIconPath(cur_cnt.app_id));
                }
                itm->AddOnKey(std::bind(&StorageContentsLayout::contents_DefaultKey, this));
                this->contents_menu->AddItem(itm);
            }
            this->contents_menu->SetSelectedIndex(0);
        }
        g_MainApplication->LoadMenuHead(cfg::Strings.GetString(189));
        this->cur_storage_id = storage_id;
    }

}