#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    ContentInformationLayout::ContentInformationLayout()
    {
        this->optionsMenu = new pu::element::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
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
            if(this->tcontents[i].IsDLC()) name = set::GetDictionaryEntry(262);
            pu::element::MenuItem *subcnt = new pu::element::MenuItem(name);
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
        msg += set::GetDictionaryEntry(160) + " ";
        std::vector<std::string> opts = { set::GetDictionaryEntry(245), set::GetDictionaryEntry(244) };
        std::string icn;
        horizon::Title cnt = this->tcontents[idx];
        if(fs::IsFile(horizon::GetExportedIconPath(cnt.ApplicationId))) icn = horizon::GetExportedIconPath(cnt.ApplicationId);
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
        msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(cnt.ApplicationId);
        msg += "\n\n" + set::GetDictionaryEntry(177) + " " + this->contents.GetFormattedTotalSize();
        msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(cnt.Version);
        if(cnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(cnt.Version >> 16) + "]";
        else
        {
            if(fs::IsFile(horizon::GetExportedIconPath(horizon::GetBaseApplicationId(cnt.ApplicationId, cnt.Type)))) icn = horizon::GetExportedIconPath(horizon::GetBaseApplicationId(cnt.ApplicationId, cnt.Type));
            horizon::TitleContents subcnts = cnt.GetContents();
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
            msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(cnt.ApplicationId);
            msg += "\n\n" + set::GetDictionaryEntry(177) + " " + subcnts.GetFormattedTotalSize();
            msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(cnt.Version);
            if(cnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(cnt.Version >> 16) + "]";
        }
        if((idx == 0) && (cnt.Location == Storage::GameCart))
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(234) }, true, icn);
            return;
        }
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
            Result rc = horizon::RemoveTitle(cnt);
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
    }

    void ContentInformationLayout::LoadContent(horizon::Title &Content)
    {
        this->tcontents.clear();
        this->tcontents.push_back(Content);
        this->contents = Content.GetContents();
        if(Content.IsBaseTitle())
        {
            std::vector<horizon::Title> tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::GameCart);
            if(!tts.empty())
            {
                for(u32 i = 0; i < tts.size(); i++)
                {
                    if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
                }
            }
            tts.clear();
            tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
            if(!tts.empty())
            {
                for(u32 i = 0; i < tts.size(); i++)
                {
                    if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
                }
            }
            tts.clear();
            tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
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
        std::string tcnt = horizon::FormatApplicationId(Content.ApplicationId);
        if(nacp != NULL)
        {
            tcnt = horizon::GetNACPName(nacp) + " (" + std::string(nacp->version) + ")";
            free(nacp);
        }
        std::string icon;
        u8 *cicon = Content.TryGetIcon();
        if(cicon != NULL)
        {
            icon = horizon::GetExportedIconPath(Content.ApplicationId);
            free(cicon);
        }
        mainapp->LoadMenuData(set::GetDictionaryEntry(187), icon, tcnt, false);
        this->UpdateElements();
    }
}