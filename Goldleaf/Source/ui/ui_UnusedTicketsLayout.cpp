#include <ui/ui_UnusedTicketsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    UnusedTicketsLayout::UnusedTicketsLayout() : pu::ui::Layout()
    {
        this->ticketsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->ticketsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->notTicketsText = new pu::ui::elm::TextBlock(0, 0, set::GetDictionaryEntry(199));
        this->notTicketsText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->notTicketsText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->notTicketsText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->notTicketsText);
        this->Add(this->ticketsMenu);
    }

    UnusedTicketsLayout::~UnusedTicketsLayout()
    {
        delete this->notTicketsText;
        delete this->ticketsMenu;
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
                std::string tname = hos::FormatApplicationId(tappid);
                pu::ui::elm::MenuItem *itm = new pu::ui::elm::MenuItem(tname);
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
        Result rc = hos::RemoveTicket(seltick);
        if(rc == 0)
        {
            mainapp->ShowNotification(set::GetDictionaryEntry(206));
            this->UpdateElements(false);
        }
        else HandleResult(rc, set::GetDictionaryEntry(207));
    }
}