
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

#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    UnusedTicketsLayout::UnusedTicketsLayout() : pu::ui::Layout()
    {
        this->ticketsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->ticketsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->ticketsMenu);
        this->notTicketsText = pu::ui::elm::TextBlock::New(0, 0, cfg::strings::Main.GetString(199));
        this->notTicketsText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->notTicketsText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->notTicketsText->SetColor(global_settings.custom_scheme.Text);
        this->Add(this->notTicketsText);
        this->Add(this->ticketsMenu);
    }

    void UnusedTicketsLayout::UpdateElements(bool Cooldown)
    {
        if(!this->tickets.empty()) this->tickets.clear();
        auto alltiks = hos::GetAllTickets();
        for(auto &ticket: alltiks)
        {
            u64 tappid = ticket.GetApplicationId();
            bool used = hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, tappid) || hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, tappid);
            if(!used) this->tickets.push_back(ticket);
        }
        global_app->LoadMenuHead(cfg::strings::Main.GetString(248));
        this->ticketsMenu->ClearItems();
        if(Cooldown) this->ticketsMenu->SetCooldownEnabled(true);
        if(this->tickets.empty())
        {
            this->notTicketsText->SetVisible(true);
            this->ticketsMenu->SetVisible(false);
        }
        else
        {
            this->notTicketsText->SetVisible(false);
            for(auto &ticket: this->tickets)
            {
                u64 tappid = ticket.GetApplicationId();
                String tname = hos::FormatApplicationId(tappid);
                auto itm = pu::ui::elm::MenuItem::New(tname);
                itm->SetColor(global_settings.custom_scheme.Text);
                itm->SetIcon(global_settings.PathForResource("/Common/Ticket.png"));
                itm->AddOnClick(std::bind(&UnusedTicketsLayout::tickets_Click, this, ticket));
                this->ticketsMenu->AddItem(itm);
            }
            this->ticketsMenu->SetSelectedIndex(0);
        }
    }

    void UnusedTicketsLayout::tickets_Click(hos::Ticket ticket)
    {
        String info = cfg::strings::Main.GetString(201) + "\n\n\n";
        u64 tappid = ticket.GetApplicationId();
        info += cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(tappid);
        info += "\n" + cfg::strings::Main.GetString(95) + " " + std::to_string(ticket.GetKeyGeneration() + 1);
        info += "\n\n";
        info += cfg::strings::Main.GetString(203);
        int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(200), info, { cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(18) }, true);
        if(sopt < 0) return;
        sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(204), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(sopt < 0) return;
        auto rc = hos::RemoveTicket(ticket);
        if(R_SUCCEEDED(rc))
        {
            global_app->ShowNotification(cfg::strings::Main.GetString(206));
            this->UpdateElements(false);
        }
        else HandleResult(rc, cfg::strings::Main.GetString(207));
    }
}