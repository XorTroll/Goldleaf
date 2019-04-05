#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    TicketManagerLayout::TicketManagerLayout() : pu::Layout()
    {
        this->ticketsMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->ticketsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->notTicketsText = new pu::element::TextBlock(450, 400, set::GetDictionaryEntry(199));
        this->notTicketsText->SetColor(gsets.CustomScheme.Text);
        this->AddChild(this->notTicketsText);
        this->AddChild(this->ticketsMenu);
    }

    void TicketManagerLayout::UpdateElements()
    {
        if(!this->tickets.empty()) this->tickets.clear();
        this->tickets = horizon::GetAllTickets();
        mainapp->LoadMenuHead(set::GetDictionaryEntry(248));
        this->ticketsMenu->ClearItems();
        this->ticketsMenu->SetCooldownEnabled(true);
        if(this->tickets.empty())
        {
            this->notTicketsText->SetVisible(true);
            this->ticketsMenu->SetVisible(false);
        }
        else
        {
            this->notTicketsText->SetVisible(false);
            for(u32 i = 0; i < this->tickets.size(); i++)
            {
                horizon::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                std::string tname = horizon::FormatApplicationId(tappid);
                pu::element::MenuItem *itm = new pu::element::MenuItem(tname);
                itm->SetColor(gsets.CustomScheme.Text);
                itm->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
                itm->AddOnClick(std::bind(&TicketManagerLayout::tickets_Click, this));
                this->ticketsMenu->AddItem(itm);
            }
            this->ticketsMenu->SetSelectedIndex(0);
        }
    }

    void TicketManagerLayout::tickets_Click()
    {
        horizon::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        std::string info = set::GetDictionaryEntry(201) + "\n\n\n";
        u64 tappid = seltick.GetApplicationId();
        info += set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(tappid);
        info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(seltick.GetKeyGeneration() + 1);
        info += "\n\n";
        bool used = horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, tappid);
        if(!used) used = horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, tappid);
        if(used) info += set::GetDictionaryEntry(202);
        else info += set::GetDictionaryEntry(203);
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), info, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(204), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        if(used)
        {
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(205), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
        }
        Result rc = es::DeleteTicket(&seltick.RId, sizeof(es::RightsId));
        if(rc == 0)
        {
            mainapp->UpdateFooter(set::GetDictionaryEntry(206));
            this->UpdateElements();
        }
        else HandleResult(rc, set::GetDictionaryEntry(207));
    }
}