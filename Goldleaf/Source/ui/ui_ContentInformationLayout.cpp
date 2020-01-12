
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

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    ContentInformationLayout::ContentInformationLayout()
    {
        this->optionsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->optionsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->optionsMenu);
        this->Add(this->optionsMenu);
    }

    void ContentInformationLayout::UpdateElements()
    {
        this->optionsMenu->ClearItems();
        if(!this->tcontents.empty()) for(u32 i = 0; i < this->tcontents.size(); i++)
        {
            String name = cfg::strings::Main.GetString(261);
            if(this->tcontents[i].IsUpdate()) name = cfg::strings::Main.GetString(262);
            if(this->tcontents[i].IsDLC()) name = cfg::strings::Main.GetString(263) + " " + std::to_string(hos::GetIdFromDLCApplicationId(this->tcontents[i].ApplicationId));
            auto subcnt = pu::ui::elm::MenuItem::New(name);
            subcnt->SetColor(global_settings.custom_scheme.Text);
            subcnt->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
            this->optionsMenu->AddItem(subcnt);
        }
        this->optionsMenu->SetSelectedIndex(0);
    }

    void ContentInformationLayout::options_Click()
    {
        u32 idx = this->optionsMenu->GetSelectedIndex();
        String msg = cfg::strings::Main.GetString(169) + "\n\n";
        msg += cfg::strings::Main.GetString(170) + " ";
        std::vector<String> opts = { cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(244), "Mount save data" };
        std::string icn;
        hos::Title cnt = this->tcontents[idx];
        if(fs::IsFile(hos::GetExportedIconPath(cnt.ApplicationId))) icn = hos::GetExportedIconPath(cnt.ApplicationId);
        switch(cnt.Type)
        {
            case ncm::ContentMetaType::Application:
                msg += cfg::strings::Main.GetString(171);
                break;
            case ncm::ContentMetaType::AddOnContent:
                msg += cfg::strings::Main.GetString(172);
                break;
            case ncm::ContentMetaType::Patch:
                msg += cfg::strings::Main.GetString(173);
                break;
            case ncm::ContentMetaType::SystemProgram:
                msg += cfg::strings::Main.GetString(174);
                break;
            case ncm::ContentMetaType::SystemData:
                msg += cfg::strings::Main.GetString(175);
                break;
            default:
                msg += cfg::strings::Main.GetString(176);
                break;
        }
        msg += "\n" + cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(cnt.ApplicationId);
        msg += "\n\n" + cfg::strings::Main.GetString(177) + " " + cnt.GetContents().GetFormattedTotalSize();
        msg += "\n\n" + cfg::strings::Main.GetString(178) + " v" + std::to_string(cnt.Version);
        if(cnt.Version != 0) msg += " [" + cfg::strings::Main.GetString(179) + " no. " + std::to_string(cnt.Version >> 16) + "]";
        if(cnt.IsBaseTitle() && (cnt.Location != Storage::NANDSystem))
        {
            msg += "\n";
            auto uid = acc::GetSelectedUser();
            hos::TitlePlayStats stats = cnt.GetGlobalPlayStats();
            if(stats.TotalPlaySeconds == 0) msg += "\n" + cfg::strings::Main.GetString(351) + "\n";
            else 
            {
                if(accountUidIsValid(&uid))
                {
                    stats = cnt.GetUserPlayStats(uid);
                    msg += "\n" + cfg::strings::Main.GetString(337);
                    msg += "\n" + cfg::strings::Main.GetString(339) + " " + hos::FormatTime(stats.SecondsFromLastLaunched);
                    msg += "\n" + cfg::strings::Main.GetString(340) + " " + hos::FormatTime(stats.TotalPlaySeconds);
                    msg += "\n";
                }
                msg += "\n" + cfg::strings::Main.GetString(338);
                msg += "\n" + cfg::strings::Main.GetString(339) + " " + hos::FormatTime(stats.SecondsFromLastLaunched);
                msg += "\n" + cfg::strings::Main.GetString(340) + " " + hos::FormatTime(stats.TotalPlaySeconds);
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
            global_app->CreateShowDialog(cfg::strings::Main.GetString(243), msg, { cfg::strings::Main.GetString(234) }, true, icn);
            return;
        }
        if(hastik) opts.push_back(cfg::strings::Main.GetString(293));
        if(cnt.Location != Storage::NANDSystem) opts.push_back(cfg::strings::Main.GetString(319));
        opts.push_back(cfg::strings::Main.GetString(18));
        int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(243), msg, opts, true, icn);
        if(sopt < 0) return;
        if(sopt == 0)
        {
            if(cnt.Location == Storage::NANDSystem)
            {
                global_app->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(185), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(186), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(sopt < 0) return;
            bool remtik = false;
            if(hastik)
            {
                int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(204), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112), cfg::strings::Main.GetString(18) }, true);
                if(sopt < 0) return;
                remtik = (sopt == 0);
            }
            Result rc = hos::RemoveTitle(cnt);
            if(R_SUCCEEDED(rc))
            {
                if(remtik) rc = hos::RemoveTicket(stik);
                if(R_SUCCEEDED(rc))
                {
                    global_app->ShowNotification(cfg::strings::Main.GetString(246));
                    global_app->ReturnToMainMenu();
                }
                else HandleResult(rc, cfg::strings::Main.GetString(247));
            }
            else HandleResult(rc, cfg::strings::Main.GetString(247));
        }
        else if(sopt == 1)
        {
            sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(182), cfg::strings::Main.GetString(184), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(sopt < 0) return;
            if(sopt == 0)
            {
                global_app->LoadLayout(global_app->GetTitleDumperLayout());
                global_app->GetTitleDumperLayout()->StartDump(cnt, hastik);
                global_app->ReturnToMainMenu();
            }
        }
        else if(sopt == 2)
        {
            if(!cnt.IsBaseTitle())
            {
                global_app->CreateShowDialog("Content save data", "This can only be done with titles, not updates or DLC.", { "Ok" }, true);
                return;
            }
            if(cnt.Location == Storage::NANDSystem)
            {
                global_app->CreateShowDialog("Content save data", "This cannot be done with system titles.", { "Ok" }, true);
                return;
            }
            if(!acc::HasUser())
            {
                global_app->CreateShowDialog("Content save data", "Select an account to browse the save data first.", { "Ok" }, true);
                return;
            }

            FsFileSystem fs;

            FsSaveDataAttribute attr = {};
            attr.application_id = cnt.ApplicationId;
            attr.uid = acc::GetSelectedUser();
            attr.save_data_type = FsSaveDataType_Account;
            auto rc = fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);

            if(R_SUCCEEDED(rc))
            {
                auto exp = new fs::FspExplorer("SaveData-0x" + hos::FormatApplicationId(cnt.ApplicationId), fs);
                global_app->GetExploreMenuLayout()->AddMountedExplorer(exp, "Game save data", icn);
                global_app->ShowNotification("Save data was mounted. Check it in the contents browsing section.");
            }
            else
            {
                HandleResult(rc, "An error ocurred attempting to mount the save data of this title and user:");
            }
        }
        else if(hastik && (sopt == 3))
        {
            sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(205), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(sopt < 0) return;
            Result rc = es::DeleteTicket(&stik.RId, sizeof(es::RightsId));
            if(R_SUCCEEDED(rc))
            {
                global_app->ShowNotification(cfg::strings::Main.GetString(206));
                this->UpdateElements();
            }
            else HandleResult(rc, cfg::strings::Main.GetString(207));
        }
        else if((hastik && sopt == 4) || (!hastik && sopt == 3))
        {
            auto rc = ns::PushLaunchVersion(cnt.ApplicationId, 0);
            if(R_SUCCEEDED(rc))
            {
                global_app->ShowNotification(cfg::strings::Main.GetString(322));
                this->UpdateElements();
            }
            else HandleResult(rc, cfg::strings::Main.GetString(234));
        }
    }

    void ContentInformationLayout::LoadContent(hos::Title &Content)
    {
        this->tcontents.clear();
        this->tcontents.push_back(Content);
        std::vector<hos::Title> tts = hos::SearchTitles(ncm::ContentMetaType::Any, Content.Location);
        for(u32 i = 0; i < tts.size(); i++)
        {
            if(Content.CheckBase(tts[i])) this->tcontents.push_back(tts[i]);
        }
        NacpStruct *nacp = Content.TryGetNACP();
        String tcnt = hos::FormatApplicationId(Content.ApplicationId);
        if(nacp != nullptr)
        {
            tcnt = hos::GetNACPName(nacp) + " (" + String(nacp->display_version) + ")";
            delete nacp;
        }
        std::string icon;
        u8 *cicon = Content.TryGetIcon();
        if(cicon != nullptr)
        {
            icon = hos::GetExportedIconPath(Content.ApplicationId);
            delete[] cicon;
            cicon = nullptr;
        }
        global_app->LoadMenuData(cfg::strings::Main.GetString(187), icon, tcnt, false);
        this->UpdateElements();
    }
}