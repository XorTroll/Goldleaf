
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

#include <ui/ui_TicketsLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <expt/expt_Export.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline std::string FormatTicketType(const hos::TicketType type) {
            switch(type) {
                case hos::TicketType::Common: {
                    return "common";
                }
                case hos::TicketType::Personalized: {
                    return "personalized";
                }
            }
            return "<invalid>";
        }

    }

    TicketsLayout::TicketsLayout() : pu::ui::Layout() {
        this->tiks_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->tiks_menu);
        this->no_unused_tiks_text = pu::ui::elm::TextBlock::New(0, 0, cfg::strings::Main.GetString(199));
        this->no_unused_tiks_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->no_unused_tiks_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->no_unused_tiks_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->no_unused_tiks_text);
        this->Add(this->tiks_menu);
    }

    void TicketsLayout::UpdateElements(bool cooldown) {
        this->tiks_menu->ClearItems();
        this->no_unused_tiks_text->SetVisible(true);
        this->tiks_menu->SetVisible(false);
        this->tiks_menu->SetCooldownEnabled(cooldown);

        g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(248));

        auto has_any = false;
        for(const auto &tik: hos::GetAllTickets()) {
            const auto tik_name = hos::FormatApplicationId(tik.rights_id.GetApplicationId());
            const auto tik_opts = "(" + FormatTicketType(tik.type) + ", " + (tik.IsUsed() ? "used" : "unused") + ")";

            auto itm = pu::ui::elm::MenuItem::New(tik_name + " " + tik_opts);
            itm->SetColor(g_Settings.custom_scheme.text);
            itm->SetIcon(g_Settings.PathForResource("/Common/Ticket.png"));
            itm->AddOnKey(std::bind(&TicketsLayout::tickets_DefaultKey, this, tik));
            this->tiks_menu->AddItem(itm);
            has_any = true;
        }

        if(has_any) {
            this->tiks_menu->SetSelectedIndex(0);
            this->no_unused_tiks_text->SetVisible(false);
            this->tiks_menu->SetVisible(true);
        }
    }

    void TicketsLayout::tickets_DefaultKey(const hos::Ticket tik) {
        const auto tik_app_id = tik.rights_id.GetApplicationId();
        const auto tik_used = tik.IsUsed();
        
        auto info = cfg::strings::Main.GetString(201) + "\n\n\n";
        
        info += cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(tik_app_id);
        info += "\n" + cfg::strings::Main.GetString(95) + " " + std::to_string(tik.rights_id.GetKeyGeneration() + 1);
        info += "\nType: " + FormatTicketType(tik.type);
        info += "\n";
        info += tik_used ? "This ticket is used" : "This ticket is not used";
        info += "\n\n";
        info += cfg::strings::Main.GetString(203);

        const auto opt_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), info, { cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(244), cfg::strings::Main.GetString(438), cfg::strings::Main.GetString(18) }, true);
        if(opt_1 == 0) {
            if(tik_used) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(440), { cfg::strings::Main.GetString(234) }, true);
            }
            else {
                const auto opt_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(204), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                if(opt_2 == 0) {
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
        }
        else if((opt_1 == 1) || (opt_1 == 2)) {
            const auto export_cert = (opt_1 == 2);

            const auto expt_tik_path = expt::ExportTicketCert(tik_app_id, export_cert);
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(439) + " '" + fs::GetSdCardExplorer()->FullPresentablePathFor(expt_tik_path) + "'");
        }
    }

}