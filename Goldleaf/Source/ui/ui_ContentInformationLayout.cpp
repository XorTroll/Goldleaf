
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    ContentInformationLayout::ContentInformationLayout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, (560 / g_Settings.menu_item_size));
        this->options_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->options_menu);
        this->Add(this->options_menu);
    }

    void ContentInformationLayout::UpdateElements() {
        this->options_menu->ClearItems();
        for(const auto &sub_cnt: this->cnt_subcontents) {
            auto name = cfg::strings::Main.GetString(261);
            if(sub_cnt.IsUpdate()) {
                name = cfg::strings::Main.GetString(262);
            }
            if(sub_cnt.IsAddOnContent()) {
                name = cfg::strings::Main.GetString(263) + " " + std::to_string(hos::GetIdFromAddOnContentApplicationId(sub_cnt.app_id));
            }
            auto sub_cnt_item = pu::ui::elm::MenuItem::New(name);
            sub_cnt_item->SetColor(g_Settings.custom_scheme.Text);
            sub_cnt_item->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
            this->options_menu->AddItem(sub_cnt_item);
        }
        this->options_menu->SetSelectedIndex(0);
    }

    void ContentInformationLayout::options_Click() {
        const auto idx = this->options_menu->GetSelectedIndex();
        auto msg = cfg::strings::Main.GetString(169) + "\n\n";
        msg += cfg::strings::Main.GetString(170) + " ";
        std::vector<String> options = { cfg::strings::Main.GetString(245), cfg::strings::Main.GetString(244), cfg::strings::Main.GetString(414) };
        std::string icon;
        const auto &sub_cnt = this->cnt_subcontents[idx];
        auto sd_exp = fs::GetSdCardExplorer();
        if(sd_exp->IsFile(hos::GetExportedIconPath(sub_cnt.app_id))) {
            icon = hos::GetExportedIconPath(sub_cnt.app_id);
        }
        switch(sub_cnt.type) {
            case NcmContentMetaType_Application: {
                msg += cfg::strings::Main.GetString(171);
                break;
            }
            case NcmContentMetaType_AddOnContent: {
                msg += cfg::strings::Main.GetString(172);
                break;
            }
            case NcmContentMetaType_Patch: {
                msg += cfg::strings::Main.GetString(173);
                break;
            }
            case NcmContentMetaType_SystemProgram: {
                msg += cfg::strings::Main.GetString(174);
                break;
            }
            case NcmContentMetaType_SystemData: {
                msg += cfg::strings::Main.GetString(175);
                break;
            }
            default: {
                msg += cfg::strings::Main.GetString(176);
                break;
            }
        }
        msg += "\n" + cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(sub_cnt.app_id);
        msg += "\n\n" + cfg::strings::Main.GetString(177) + " " + fs::FormatSize(sub_cnt.GetContents().GetTotalSize());
        msg += "\n\n" + cfg::strings::Main.GetString(178) + " v" + std::to_string(sub_cnt.version);
        if(sub_cnt.version != 0) {
            msg += " [" + cfg::strings::Main.GetString(179) + " no. " + std::to_string(sub_cnt.version >> 16) + "]";
        }

        if(sub_cnt.IsBaseTitle() && (sub_cnt.storage_id != NcmStorageId_BuiltInSystem)) {
            msg += "\n";
            // TODO: show both global and user stats?
            auto stats = sub_cnt.GetGlobalPlayStats();
            if(stats.total_play_secs == 0) {
                msg += "\n" + cfg::strings::Main.GetString(351) + "\n";
            }
            else  {
                if(acc::HasSelectedUser()) {
                    stats = sub_cnt.GetUserPlayStats(acc::GetSelectedUser());
                    msg += "\n" + cfg::strings::Main.GetString(337);
                    msg += "\n" + cfg::strings::Main.GetString(339) + " " + hos::FormatTime(stats.secs_from_last_launched);
                    msg += "\n" + cfg::strings::Main.GetString(340) + " " + hos::FormatTime(stats.total_play_secs);
                    msg += "\n";
                }
                msg += "\n" + cfg::strings::Main.GetString(338);
                msg += "\n" + cfg::strings::Main.GetString(339) + " " + hos::FormatTime(stats.secs_from_last_launched);
                msg += "\n" + cfg::strings::Main.GetString(340) + " " + hos::FormatTime(stats.total_play_secs);
            }
        }
        const auto tiks = hos::GetAllTickets();
        const auto it = std::find_if(tiks.begin(), tiks.end(), [&](const hos::Ticket &tik) {
            return tik.GetApplicationId() == sub_cnt.app_id;
        });

        const bool has_tik = it != tiks.end();
        hos::Ticket tik = {};
        if(has_tik) {
            tik = *it;
        }

        if(sub_cnt.storage_id == NcmStorageId_GameCard) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), msg, { cfg::strings::Main.GetString(234) }, true, icon);
            return;
        }

        if(has_tik) {
            options.push_back(cfg::strings::Main.GetString(293));
        }
        if(sub_cnt.storage_id != NcmStorageId_BuiltInSystem) {
            options.push_back(cfg::strings::Main.GetString(319));
        }
        options.push_back(cfg::strings::Main.GetString(18));

        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), msg, options, true, icon);
        if(option_1 == 0) {
            if(sub_cnt.storage_id == NcmStorageId_BuiltInSystem) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(185), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(186), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            auto remove_tik = false;
            if(has_tik) {
                const auto option_3 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(204), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112), cfg::strings::Main.GetString(18) }, true);
                remove_tik = option_3 == 0;
            }
            auto rc = hos::RemoveTitle(sub_cnt);
            if(R_SUCCEEDED(rc)) {
                if(remove_tik) {
                    rc = hos::RemoveTicket(tik);
                }
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(246));
                    g_MainApplication->ReturnToMainMenu();
                }
            }
            if(R_FAILED(rc)) {
                HandleResult(rc, cfg::strings::Main.GetString(247));
            }
        }
        else if(option_1 == 1) {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(182), cfg::strings::Main.GetString(184), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
                g_MainApplication->LoadLayout(g_MainApplication->GetTitleDumperLayout());
                g_MainApplication->GetTitleDumperLayout()->StartDump(sub_cnt, has_tik);
                g_MainApplication->LoadLayout(g_MainApplication->GetContentInformationLayout());
            }
        }
        else if(option_1 == 2) {
            if(!sub_cnt.IsBaseTitle()) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(408), cfg::strings::Main.GetString(409), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            if(sub_cnt.storage_id == NcmStorageId_BuiltInSystem) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(408), cfg::strings::Main.GetString(410), { cfg::strings::Main.GetString(234) }, true);
                return;
            }
            if(!acc::HasSelectedUser()) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(408), cfg::strings::Main.GetString(411), { cfg::strings::Main.GetString(234) }, true);
                return;
            }

            // TODO: maybe migrate this to fs:: namespace?
            FsFileSystem fs;

            const FsSaveDataAttribute attr = {
                .application_id = sub_cnt.app_id,
                .uid = acc::GetSelectedUser(),
                .save_data_type = FsSaveDataType_Account,
            };
            auto rc = fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);
            if(R_SUCCEEDED(rc)) {
                auto exp = new fs::FspExplorer("SaveData-0x" + hos::FormatApplicationId(sub_cnt.app_id), fs);
                exp->SetShouldWarnOnWriteAccess(true);
                g_MainApplication->GetExploreMenuLayout()->AddMountedExplorer(exp, cfg::strings::Main.GetString(408), icon);
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(412));
            }
            else {
                HandleResult(rc, cfg::strings::Main.GetString(413));
            }
        }
        else if((option_1 == 3) && has_tik) {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(200), cfg::strings::Main.GetString(205), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
                const auto rc = es::DeleteTicket(tik.rights_id);
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(206));
                    this->UpdateElements();
                }
                else {
                    HandleResult(rc, cfg::strings::Main.GetString(207));
                }
            }
        }
        else if(((option_1 == 3) && !has_tik) || ((option_1 == 4) && has_tik)) {
            const auto rc = hos::UpdateTitleVersion(sub_cnt); // ns::PushLaunchVersion(sub_cnt.app_id, 0);
            if(R_SUCCEEDED(rc)) {
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(322));
                this->UpdateElements();
            }
            else {
                HandleResult(rc, cfg::strings::Main.GetString(234));
            }
        }
    }

    void ContentInformationLayout::LoadContent(const hos::Title &content) {
        this->cnt_subcontents.clear();
        this->cnt_subcontents.push_back(content);
        const auto &all_contents = hos::SearchTitles(NcmContentMetaType_Unknown, content.storage_id);
        for(const auto &cnt: all_contents) {
            if(content.IsBaseOf(cnt)) {
                this->cnt_subcontents.push_back(cnt);
            }
        }
        String head = hos::FormatApplicationId(content.app_id);
        const auto nacp = content.TryGetNACP();
        if(!hos::IsNacpEmpty(nacp)) {
            head = hos::FindNacpName(nacp) + " (" + nacp.display_version + ")";
        }
        std::string icon;
        auto icon_data = content.TryGetIcon();
        if(icon_data != nullptr) {
            icon = hos::GetExportedIconPath(content.app_id);
            delete[] icon_data;
        }
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(187), icon, head, false);
        this->UpdateElements();
    }

}