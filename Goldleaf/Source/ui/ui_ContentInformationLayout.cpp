
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

#include <ui/ui_ContentInformationLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    ContentInformationLayout::ContentInformationLayout()
    {
        this->optionsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optionsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->optionsMenu);
        this->Add(this->optionsMenu);
    }

    void ContentInformationLayout::UpdateElements()
    {
        this->optionsMenu->ClearItems();
        if(!this->tcontents.empty()) for(u32 i = 0; i < this->tcontents.size(); i++)
        {
            pu::String name = set::GetDictionaryEntry(261);
            if(this->tcontents[i].IsUpdate()) name = set::GetDictionaryEntry(262);
            if(this->tcontents[i].IsDLC()) name = set::GetDictionaryEntry(263) + " " + std::to_string(hos::GetIdFromDLCApplicationId(this->tcontents[i].ApplicationId));
            auto subcnt = pu::ui::elm::MenuItem::New(name);
            subcnt->SetColor(gsets.CustomScheme.Text);
            subcnt->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
            this->optionsMenu->AddItem(subcnt);
        }
        this->optionsMenu->SetSelectedIndex(0);
    }

    void ContentInformationLayout::options_Click()
    {
        u32 idx = this->optionsMenu->GetSelectedIndex();
        pu::String msg = set::GetDictionaryEntry(169) + "\n\n";
        msg += set::GetDictionaryEntry(170) + " ";
        std::vector<pu::String> opts = { set::GetDictionaryEntry(245), set::GetDictionaryEntry(244) };
        std::string icn;
        hos::Title cnt = this->tcontents[idx];
        if(fs::IsFile(hos::GetExportedIconPath(cnt.ApplicationId))) icn = hos::GetExportedIconPath(cnt.ApplicationId);
        switch(cnt.Type)
        {
            case NcmContentMetaType_Application:
                msg += set::GetDictionaryEntry(171);
                break;
            case NcmContentMetaType_AddOnContent:
                msg += set::GetDictionaryEntry(172);
                break;
            case NcmContentMetaType_Patch:
                msg += set::GetDictionaryEntry(173);
                break;
            case NcmContentMetaType_SystemProgram:
                msg += set::GetDictionaryEntry(174);
                break;
            case NcmContentMetaType_SystemData:
                msg += set::GetDictionaryEntry(175);
                break;
            default:
                msg += set::GetDictionaryEntry(176);
                break;
        }
        msg += "\n" + set::GetDictionaryEntry(90) + " " + hos::FormatApplicationId(cnt.ApplicationId);
        msg += "\n\n" + set::GetDictionaryEntry(177) + " " + cnt.GetContents().GetFormattedTotalSize();
        msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(cnt.Version);
        if(cnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(cnt.Version >> 16) + "]";
        if(cnt.IsBaseTitle() && (cnt.Location != Storage::NANDSystem))
        {
            msg += "\n";
            auto uid = acc::GetSelectedUser();
            hos::TitlePlayStats stats = cnt.GetGlobalPlayStats();
            if(stats.TotalPlaySeconds == 0) msg += "\n" + set::GetDictionaryEntry(351) + "\n";
            else 
            {
                if(accountUidIsValid(&uid))
                {
                    stats = cnt.GetUserPlayStats(uid);
                    msg += "\n" + set::GetDictionaryEntry(337);
                    msg += "\n" + set::GetDictionaryEntry(339) + " " + hos::FormatTime(stats.SecondsFromLastLaunched);
                    msg += "\n" + set::GetDictionaryEntry(340) + " " + hos::FormatTime(stats.TotalPlaySeconds);
                    msg += "\n";
                }
                msg += "\n" + set::GetDictionaryEntry(338);
                msg += "\n" + set::GetDictionaryEntry(339) + " " + hos::FormatTime(stats.SecondsFromLastLaunched);
                msg += "\n" + set::GetDictionaryEntry(340) + " " + hos::FormatTime(stats.TotalPlaySeconds);
            }
        }
        auto tiks = hos::GetAllTickets();
        bool hastik = false;
        hos::Ticket stik;

        auto it = std::find_if(tiks.begin(), tiks.end(), [&](hos::Ticket &tik) -> bool
        {
            return (tik.GetApplicationId() == cnt.ApplicationId);
        });

        if(it != tiks.end())
        {
            hastik = true;
            stik = *it;
        }

        if(cnt.Location == Storage::GameCart)
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(234) }, true, icn);
            return;
        }
        if(hastik) opts.push_back(set::GetDictionaryEntry(293));
        if(cnt.Location != Storage::NANDSystem) opts.push_back(set::GetDictionaryEntry(319));
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
            bool remtik = false;
            if(hastik)
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(204), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                remtik = (sopt == 0);
            }
            Result rc = hos::RemoveTitle(cnt);
            if(rc == 0)
            {
                if(remtik) rc = hos::RemoveTicket(stik);
                if(rc == 0)
                {
                    mainapp->ShowNotification(set::GetDictionaryEntry(246));
                    mainapp->UnloadMenuData();
                    mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                }
                else HandleResult(rc, set::GetDictionaryEntry(247));
            }
            else HandleResult(rc, set::GetDictionaryEntry(247));
        }
        else if(sopt == 1)
        {
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(182), set::GetDictionaryEntry(184), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            if(sopt == 0)
            {
                mainapp->LoadLayout(mainapp->GetTitleDumperLayout());
                mainapp->GetTitleDumperLayout()->StartDump(cnt, hastik);
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
        }
        else if(hastik && (sopt == 2))
        {
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(205), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            Result rc = es::DeleteTicket(&stik.RId, sizeof(es::RightsId));
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(206));
                this->UpdateElements();
            }
            else HandleResult(rc, set::GetDictionaryEntry(207));
        }
        else if((hastik && sopt == 3) || (!hastik && sopt == 2))
        {
            auto rc = ns::PushLaunchVersion(cnt.ApplicationId, 0);
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(322));
                this->UpdateElements();
            }
            else HandleResult(rc, set::GetDictionaryEntry(234));
        }
    }

    void ContentInformationLayout::LoadContent(hos::Title &Content)
    {
        this->tcontents.clear();
        this->tcontents.push_back(Content);
        std::vector<hos::Title> tts = hos::SearchTitles(NcmContentMetaType_Unknown, Content.Location);
        for(u32 i = 0; i < tts.size(); i++)
        {
            if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
        }
        NacpStruct *nacp = Content.TryGetNACP();
        pu::String tcnt = hos::FormatApplicationId(Content.ApplicationId);
        if(nacp != NULL)
        {
            tcnt = hos::GetNACPName(nacp) + " (" + pu::String(nacp->version) + ")";
            delete nacp;
        }
        std::string icon;
        u8 *cicon = Content.TryGetIcon();
        if(cicon != NULL)
        {
            icon = hos::GetExportedIconPath(Content.ApplicationId);
            delete[] cicon;
            cicon = NULL;
        }
        mainapp->LoadMenuData(set::GetDictionaryEntry(187), icon, tcnt, false);
        this->UpdateElements();
    }
}