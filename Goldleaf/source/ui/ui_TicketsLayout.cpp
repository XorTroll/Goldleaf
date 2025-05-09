
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

#include <ui/ui_TicketsLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <expt/expt_Export.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline std::string FormatTicketType(const cnt::TicketType type) {
            switch(type) {
                case cnt::TicketType::Common: {
                    return cfg::Strings.GetString(448);
                }
                case cnt::TicketType::Personalized: {
                    return cfg::Strings.GetString(449);
                }
            }
            return cfg::Strings.GetString(442);
        }

    }

    void TicketsLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    TicketsLayout::TicketsLayout() : pu::ui::Layout() {
        this->tiks_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->tiks_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        this->no_unused_tiks_text = pu::ui::elm::TextBlock::New(0, 0, cfg::Strings.GetString(199));
        this->no_unused_tiks_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->no_unused_tiks_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->no_unused_tiks_text->SetColor(g_Settings.GetColorScheme().text);
        this->Add(this->no_unused_tiks_text);
        this->Add(this->tiks_menu);

        this->SetOnInput(std::bind(&TicketsLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void TicketsLayout::UpdateElements() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(4), CommonIconKind::Ticket, cfg::Strings.GetString(279));
        this->tiks_menu->ClearItems();
        this->no_unused_tiks_text->SetVisible(true);
        this->tiks_menu->SetVisible(false);

        g_MainApplication->LoadMenuHead(cfg::Strings.GetString(248));

        auto has_any = false;
        for(const auto &tik: cnt::GetTickets()) {
            auto tik_name = util::FormatApplicationId(esGetRightsIdApplicationId(&tik.rights_id));

            const auto used_title = tik.IsUsed();
            const auto is_used = used_title.has_value();
            if(is_used) {
                tik_name = used_title->get().cache.display_name;
            }

            const auto tik_opts = "(" + FormatTicketType(tik.type) + ", " + (is_used ? cfg::Strings.GetString(450) : cfg::Strings.GetString(451)) + ")";

            auto itm = pu::ui::elm::MenuItem::New(tik_name + " " + tik_opts);
            itm->SetColor(g_Settings.GetColorScheme().text);
            itm->SetIcon(GetCommonIcon(CommonIconKind::Ticket));
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

    void TicketsLayout::tickets_DefaultKey(const cnt::Ticket tik) {
        const auto tik_app_id = esGetRightsIdApplicationId(&tik.rights_id);
        const auto used_title = tik.IsUsed();
        const auto tik_used = used_title.has_value();
        
        auto info = cfg::Strings.GetString(201) + "\n\n\n";
        
        info += cfg::Strings.GetString(90) + " " + util::FormatApplicationId(tik_app_id);
        if(tik_used) {
            info += " (" + used_title->get().cache.display_name + ")";
        }

        const auto key_gen = esGetRightsIdKeyGeneration(&tik.rights_id);
        info += "\n" + cfg::Strings.GetString(95) + " " + std::to_string(key_gen) + " (" + cnt::GetKeyGenerationRange(key_gen) + ")";
        info += "\n" + cfg::Strings.GetString(447) + " " + FormatTicketType(tik.type);
        info += "\n\n";
        info += tik_used ? cfg::Strings.GetString(202) : cfg::Strings.GetString(203);

        const auto opt_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(200), info, { cfg::Strings.GetString(245), cfg::Strings.GetString(244), cfg::Strings.GetString(438), cfg::Strings.GetString(18) }, true);
        if(opt_1 == 0) {
            if(tik_used) {
                g_MainApplication->DisplayDialog(cfg::Strings.GetString(200), cfg::Strings.GetString(440), { cfg::Strings.GetString(234) }, true);
            }
            else {
                const auto opt_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(200), cfg::Strings.GetString(204), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                if(opt_2 == 0) {
                    const auto rc = cnt::RemoveTicket(tik);
                    if(R_SUCCEEDED(rc)) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(206));
                        this->UpdateElements();
                    }
                    else {
                        HandleResult(rc, cfg::Strings.GetString(207));
                    }   
                }
            }
        }
        else if((opt_1 == 1) || (opt_1 == 2)) {
            const auto export_cert = (opt_1 == 2);

            const auto expt_tik_path = expt::ExportTicketCert(tik_app_id, export_cert);
            g_MainApplication->ShowNotification(cfg::Strings.GetString(439) + " '" + fs::GetSdCardExplorer()->FullPresentablePathFor(expt_tik_path) + "'");
        }
    }

}
