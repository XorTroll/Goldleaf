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
        this->baseTitleItem = new pu::element::MenuItem(set::GetDictionaryEntry(261));
        this->baseTitleItem->SetColor(gsets.CustomScheme.Text);
        this->baseTitleItem->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
        this->optionsMenu->AddItem(this->baseTitleItem);
        if(!this->subcnts.empty()) for(u32 i = 0; i < this->subcnts.size(); i++)
        {
            pu::element::MenuItem *subcnt = new pu::element::MenuItem(this->subcnts[i].IsUpdate() ? set::GetDictionaryEntry(262) : set::GetDictionaryEntry(263));
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
        std::vector<std::string> opts = { set::GetDictionaryEntry(245) };
        std::string icn;
        horizon::Title subcnt;
        if(idx == 0)
        {
            if(fs::IsFile(horizon::GetExportedIconPath(this->content.ApplicationId))) icn = horizon::GetExportedIconPath(this->content.ApplicationId);
            opts.push_back(set::GetDictionaryEntry(244));
            switch(this->content.Type)
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
            msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(this->content.ApplicationId);
            msg += "\n\n" + set::GetDictionaryEntry(177) + " " + this->contents.GetFormattedTotalSize();
            msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(this->content.Version);
            if(this->content.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(this->content.Version >> 16) + "]";
        }
        else
        {
            subcnt = this->subcnts[idx - 1];
            if(fs::IsFile(horizon::GetExportedIconPath(horizon::GetBaseApplicationId(subcnt.ApplicationId, subcnt.Type)))) icn = horizon::GetExportedIconPath(horizon::GetBaseApplicationId(subcnt.ApplicationId, subcnt.Type));
            horizon::TitleContents subcnts = subcnt.GetContents();
            switch(subcnt.Type)
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
            msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(subcnt.ApplicationId);
            msg += "\n\n" + set::GetDictionaryEntry(177) + " " + subcnts.GetFormattedTotalSize();
            msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(subcnt.Version);
            if(subcnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(subcnt.Version >> 16) + "]";
        }
        if((idx == 0) && (this->content.Location == Storage::GameCart))
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(234) }, true, icn);
            return;
        }
        opts.push_back(set::GetDictionaryEntry(18));
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, opts, true, icn);
        if(sopt < 0) return;
        if(idx == 0)
        {
            if(sopt == 0)
            {
                if(this->content.Location == Storage::NANDSystem)
                {
                    mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(185), { set::GetDictionaryEntry(234) }, true);
                    return;
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(186), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                Result rc = horizon::RemoveTitle(this->content);
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
                    mainapp->GetTitleDumperLayout()->StartDump(this->content);
                    mainapp->UnloadMenuData();
                    mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                }
            }
        }
        else
        {
            if(sopt == 0)
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(186), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                Result rc = horizon::RemoveTitle(subcnt);
                if(rc == 0)
                {
                    mainapp->ShowNotification(set::GetDictionaryEntry(246));
                    mainapp->UnloadMenuData();
                    mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                }
                else HandleResult(rc, set::GetDictionaryEntry(247));
            }
        }
    }

    void ContentInformationLayout::LoadContent(horizon::Title &Content)
    {
        this->subcnts.clear();
        this->content = Content;
        this->contents = Content.GetContents();
        bool hasupd = false;
        bool hasdlc = false;
        std::vector<horizon::Title> tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::GameCart);
        if(!tts.empty())
        {
            for(u32 i = 0; i < tts.size(); i++)
            {
                if(Content.CheckBase(tts[i]))
                {
                    if(tts[i].IsUpdate()) hasupd = true;
                    if(tts[i].IsDLC()) hasdlc = true;
                    this->subcnts.push_back(tts[i]);
                }
            }
        }
        tts.clear();
        tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
        if(!tts.empty())
        {
            for(u32 i = 0; i < tts.size(); i++)
            {
                if(Content.CheckBase(tts[i]))
                {
                    if(tts[i].IsUpdate()) hasupd = true;
                    if(tts[i].IsDLC()) hasdlc = true;
                    this->subcnts.push_back(tts[i]);
                }
            }
        }
        tts.clear();
        tts = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
        if(!tts.empty())
        {
            for(u32 i = 0; i < tts.size(); i++)
            {
                if(Content.CheckBase(tts[i]))
                {
                    if(tts[i].IsUpdate()) hasupd = true;
                    if(tts[i].IsDLC()) hasdlc = true;
                    this->subcnts.push_back(tts[i]);
                }
            }
        }
        tts.clear();
        NacpStruct *nacp = Content.TryGetNACP();
        std::string tcnt = horizon::FormatApplicationId(Content.ApplicationId);
        if(nacp != NULL)
        {
            tcnt = horizon::GetNACPName(nacp);
            tcnt += " (" + std::string(nacp->version);
            if(hasupd)
            {
                if(hasdlc) tcnt += ", " + set::GetDictionaryEntry(267) + ")";
                else tcnt += ", " + set::GetDictionaryEntry(268) + ")";
            }
            else
            {
                if(hasdlc) tcnt += ", " + set::GetDictionaryEntry(269) + ")";
                else tcnt += ")";
            }
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