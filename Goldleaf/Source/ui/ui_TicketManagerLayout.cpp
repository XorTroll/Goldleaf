#include <ui/ui_TicketManagerLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    TicketManagerLayout::TicketManagerLayout() : pu::ui::Layout()
    {
        this->ticketsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->ticketsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->notTicketsText = new pu::ui::elm::TextBlock(450, 400, set::GetDictionaryEntry(199));
        this->notTicketsText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->notTicketsText);
        this->Add(this->ticketsMenu);
    }

    TicketManagerLayout::~TicketManagerLayout()
    {
        delete this->notTicketsText;
        delete this->ticketsMenu;
    }

    void TicketManagerLayout::UpdateElements()
    {
        if(!this->tickets.empty()) this->tickets.clear();
        this->tickets = hos::GetAllTickets();
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
                hos::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                bool used = hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, tappid);
                if(!used) used = hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, tappid);
                if(used) continue;
                std::string tname = hos::FormatApplicationId(tappid);
                pu::ui::elm::MenuItem *itm = new pu::ui::elm::MenuItem(tname);
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
        hos::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        std::string info = set::GetDictionaryEntry(201) + "\n\n\n";
        u64 tappid = seltick.GetApplicationId();
        info += set::GetDictionaryEntry(90) + " " + hos::FormatApplicationId(tappid);
        info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(seltick.GetKeyGeneration() + 1);
        info += "\n\n";
        info += set::GetDictionaryEntry(203);
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), info, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(204), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        Result rc = es::DeleteTicket(&seltick.RId, sizeof(es::RightsId));
        if(rc == 0)
        {
            mainapp->ShowNotification(set::GetDictionaryEntry(206));
            this->UpdateElements();
        }
        else HandleResult(rc, set::GetDictionaryEntry(207));
    }
}