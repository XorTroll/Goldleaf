
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

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    UnusedTicketsLayout::UnusedTicketsLayout() : pu::ui::Layout()
    {
        this->ticketsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->ticketsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->ticketsMenu);
        this->notTicketsText = pu::ui::elm::TextBlock::New(0, 0, set::GetDictionaryEntry(199));
        this->notTicketsText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->notTicketsText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->notTicketsText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->notTicketsText);
        this->Add(this->ticketsMenu);
    }

    void UnusedTicketsLayout::UpdateElements(bool Cooldown)
    {
        if(!this->tickets.empty()) this->tickets.clear();
        auto alltiks = hos::GetAllTickets();
        for(u32 i = 0; i < alltiks.size(); i++)
        {
            hos::Ticket ticket = alltiks[i];
            u64 tappid = ticket.GetApplicationId();
            bool used = hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, tappid);
            if(!used) used = hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, tappid);
            if(!used) this->tickets.push_back(alltiks[i]);
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(248));
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
            std::vector<u32> usedidxs;
            for(u32 i = 0; i < this->tickets.size(); i++)
            {
                hos::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                pu::String tname = hos::FormatApplicationId(tappid);
                auto itm = pu::ui::elm::MenuItem::New(tname);
                itm->SetColor(gsets.CustomScheme.Text);
                itm->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
                itm->AddOnClick(std::bind(&UnusedTicketsLayout::tickets_Click, this));
                this->ticketsMenu->AddItem(itm);
            }
            this->ticketsMenu->SetSelectedIndex(0);
        }
    }

    void UnusedTicketsLayout::tickets_Click()
    {
        hos::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        pu::String info = set::GetDictionaryEntry(201) + "\n\n\n";
        u64 tappid = seltick.GetApplicationId();
        info += set::GetDictionaryEntry(90) + " " + hos::FormatApplicationId(tappid);
        info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(seltick.GetKeyGeneration() + 1);
        info += "\n\n";
        info += set::GetDictionaryEntry(203);
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), info, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(204), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        Result rc = hos::RemoveTicket(seltick);
        if(rc == 0)
        {
            mainapp->ShowNotification(set::GetDictionaryEntry(206));
            this->UpdateElements(false);
        }
        else HandleResult(rc, set::GetDictionaryEntry(207));
    }
}