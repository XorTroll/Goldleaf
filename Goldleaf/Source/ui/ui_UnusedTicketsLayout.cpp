
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

#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    UnusedTicketsLayout::UnusedTicketsLayout() : pu::ui::Layout() {
        this->tiks_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, (560 / g_Settings.menu_item_size));
        this->tiks_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->tiks_menu);
        this->no_unused_tiks_text = pu::ui::elm::TextBlock::New(0, 0, cfg::strings::Main.GetString(199));
        this->no_unused_tiks_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->no_unused_tiks_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->no_unused_tiks_text->SetColor(g_Settings.custom_scheme.Text);
        this->Add(this->no_unused_tiks_text);
        this->Add(this->tiks_menu);
    }

    void UnusedTicketsLayout::UpdateElements(bool cooldown) {
        this->tiks_menu->ClearItems();
        this->no_unused_tiks_text->SetVisible(true);
        this->tiks_menu->SetVisible(false);
        this->tiks_menu->SetCooldownEnabled(cooldown);

        auto tiks = hos::GetAllTickets();
        auto menu_ready = false;
        for(const auto &tik: tiks) {
            const auto tik_app_id = tik.GetApplicationId();
            const auto is_tik_used = hos::ExistsTitle(NcmContentMetaType_Unknown, Storage::SdCard, tik_app_id) || hos::ExistsTitle(NcmContentMetaType_Unknown, Storage::NANDUser, tik_app_id);
            if(!is_tik_used) {
                auto tik_name = hos::FormatApplicationId(tik_app_id);
                auto itm = pu::ui::elm::MenuItem::New(tik_name);
                itm->SetColor(g_Settings.custom_scheme.Text);
                itm->SetIcon(g_Settings.PathForResource("/Common/Ticket.png"));
                itm->AddOnClick(std::bind(&UnusedTicketsLayout::tickets_Click, this, tik));
                this->tiks_menu->AddItem(itm);

                if(!menu_ready) {
                    this->no_unused_tiks_text->SetVisible(false);
                    this->tiks_menu->SetVisible(true);
                    this->tiks_menu->SetSelectedIndex(0);
                    menu_ready = true;
                }
            }
        }
    }

    void UnusedTicketsLayout::tickets_Click(const hos::Ticket &tik) {
        auto info = cfg::strings::Main.GetString(201) + "\n\n\n";
        const auto tik_app_id = tik.GetApplicationId();
        info += cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(tik_app_id);
        info += "\n" + cfg::strings::Main.GetString(95) + " " + std::to_string(tik.GetKeyGeneration() + 1);
        info += "\n\n";
        info += cfg::strings::Main.GetString(203);
        int sopt = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), info, { cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(18) }, true);
        if(sopt < 0) return;
        sopt = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(204), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(sopt < 0) return;
        const auto rc = hos::RemoveTicket(tik);
        if(R_SUCCEEDED(rc)) {
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(206));
            this->UpdateElements(false);
        }
        else {
            HandleResult(rc, cfg::strings::Main.GetString(207));
        }
    }

}