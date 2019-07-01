#include <ui/ui_ContentInformationLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    ContentInformationLayout::ContentInformationLayout()
    {
        this->optionsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optionsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->Add(this->optionsMenu);
    }

    ContentInformationLayout::~ContentInformationLayout()
    {
        delete this->optionsMenu;
    }

    void ContentInformationLayout::UpdateElements()
    {
        this->optionsMenu->ClearItems();
        if(!this->tcontents.empty()) for(u32 i = 0; i < this->tcontents.size(); i++)
        {
            std::string name = set::GetDictionaryEntry(261);
            if(this->tcontents[i].IsUpdate()) name = set::GetDictionaryEntry(262);
            if(this->tcontents[i].IsDLC()) name = set::GetDictionaryEntry(263);
            pu::ui::elm::MenuItem *subcnt = new pu::ui::elm::MenuItem(name);
            subcnt->SetColor(gsets.CustomScheme.Text);
            subcnt->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
            this->optionsMenu->AddItem(subcnt);
        }
        this->optionsMenu->SetSelectedIndex(0);
    }

    void ContentInformationLayout::options_Click()
    {
        u32 idx = this->optionsMenu->GetSelectedIndex();
        std::string msg = set::GetDictionaryEntry(169) + "\n\n";
        msg += set::GetDictionaryEntry(170) + " ";
        std::vector<pu::String> opts = { set::GetDictionaryEntry(245), set::GetDictionaryEntry(244) };
        std::string icn;
        hos::Title cnt = this->tcontents[idx];
        if(fs::IsFile(hos::GetExportedIconPath(cnt.ApplicationId))) icn = hos::GetExportedIconPath(cnt.ApplicationId);
        switch(cnt.Type)
        {
            case ncm::ContentMetaType::Application:
                msg += set::GetDictionaryEntry(171);
                break;
            case ncm::ContentMetaType::AddOnContent:
                msg += set::GetDictionaryEntry(172);
                break;
            case ncm::ContentMetaType::Patch:
                msg += set::GetDictionaryEntry(173);
                break;
            case ncm::ContentMetaType::SystemProgram:
                msg += set::GetDictionaryEntry(174);
                break;
            case ncm::ContentMetaType::SystemData:
                msg += set::GetDictionaryEntry(175);
                break;
            default:
                msg += set::GetDictionaryEntry(176);
                break;
        }
        msg += "\n" + set::GetDictionaryEntry(90) + " " + hos::FormatApplicationId(cnt.ApplicationId);
        msg += "\n\n" + set::GetDictionaryEntry(177) + " " + this->contents.GetFormattedTotalSize();
        msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(cnt.Version);
        if(cnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(cnt.Version >> 16) + "]";
        auto tiks = hos::GetAllTickets();
        bool hastik = false;
        int tikidx = -1;
        for(u32 i = 0; i < tiks.size(); i++)
        {
            if(tiks[i].GetApplicationId() == cnt.ApplicationId)
            {
                msg += "\n\nTicket found.\nID: " + tiks[i].ToString();
                hastik = true;
                tikidx = i;
                break;
            }
        }
        if((idx == 0) && (cnt.Location == Storage::GameCart))
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(234) }, true, icn);
            return;
        }
        if(hastik) opts.push_back("Remove ticket");
        opts.push_back(set::GetDictionaryEntry(18));
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, opts, true, icn);
        if(sopt < 0) return;
        if(sopt == 0)
        {
            if(cnt.Location == Storage::NANDSystem)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(185), { set::GetDictionaryEntry(234) }, true);
                return;
            }
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(186), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            Result rc = hos::RemoveTitle(cnt);
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(246));
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
            else HandleResult(rc, set::GetDictionaryEntry(247));
        }
        else if(sopt == 1)
        {
            if(this->contents.GetTotalSize() >= 0x100000000)
            {
                sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(182), set::GetDictionaryEntry(183), { set::GetDictionaryEntry(234), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
            }
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(182), set::GetDictionaryEntry(184), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            if(sopt == 0)
            {
                mainapp->LoadLayout(mainapp->GetTitleDumperLayout());
                mainapp->GetTitleDumperLayout()->StartDump(cnt);
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
        }
        else if(hastik && (sopt == 2))
        {
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(205), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            Result rc = es::DeleteTicket(&tiks[tikidx].RId, sizeof(es::RightsId));
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(206));
                this->UpdateElements();
            }
            else HandleResult(rc, set::GetDictionaryEntry(207));
        }
    }

    void ContentInformationLayout::LoadContent(hos::Title &Content)
    {
        this->tcontents.clear();
        this->tcontents.push_back(Content);
        this->contents = Content.GetContents();
        if(Content.IsBaseTitle())
        {
            std::vector<hos::Title> tts = hos::SearchTitles(ncm::ContentMetaType::Any, Storage::GameCart);
            if(!tts.empty())
            {
                for(u32 i = 0; i < tts.size(); i++)
                {
                    if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
                }
            }
            tts.clear();
            tts = hos::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
            if(!tts.empty())
            {
                for(u32 i = 0; i < tts.size(); i++)
                {
                    if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
                }
            }
            tts.clear();
            tts = hos::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
            if(!tts.empty())
            {
                for(u32 i = 0; i < tts.size(); i++)
                {
                    if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
                }
            }
            tts.clear();
        }
        NacpStruct *nacp = Content.TryGetNACP();
        std::string tcnt = hos::FormatApplicationId(Content.ApplicationId);
        if(nacp != NULL)
        {
            tcnt = hos::GetNACPName(nacp) + " (" + std::string(nacp->version) + ")";
            free(nacp);
        }
        std::string icon;
        u8 *cicon = Content.TryGetIcon();
        if(cicon != NULL)
        {
            icon = hos::GetExportedIconPath(Content.ApplicationId);
            free(cicon);
            cicon = NULL;
        }
        mainapp->LoadMenuData(set::GetDictionaryEntry(187), icon, tcnt, false);
        this->UpdateElements();
    }
}