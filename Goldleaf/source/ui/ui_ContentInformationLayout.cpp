
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

namespace {

    inline std::string FormatPlayStatsInfo(const std::string &kind, const hos::TitlePlayStats &stats) {
        std::string fmt;
        fmt += "\n" + kind;
        fmt += "\n" + cfg::strings::Main.GetString(339) + " " + hos::FormatTime(stats.secs_from_last_launched);
        fmt += "\n" + cfg::strings::Main.GetString(340) + " " + hos::FormatTime(stats.total_play_secs);
        return fmt;
    }

}

namespace ui {

    ContentInformationLayout::ContentInformationLayout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
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
            sub_cnt_item->SetColor(g_Settings.custom_scheme.text);
            sub_cnt_item->AddOnKey(std::bind(&ContentInformationLayout::options_DefaultKey, this));
            this->options_menu->AddItem(sub_cnt_item);
        }
        this->options_menu->SetSelectedIndex(0);
    }

    void ContentInformationLayout::options_DefaultKey() {
        const auto idx = this->options_menu->GetSelectedIndex();
        auto msg = cfg::strings::Main.GetString(169) + "\n\n";
        msg += cfg::strings::Main.GetString(170) + " ";
        std::string icon;
        const auto &sub_cnt = this->cnt_subcontents.at(idx);
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

            const auto global_stats = sub_cnt.GetGlobalPlayStats();
            if(global_stats.total_play_secs == 0) {
                msg += "\n" + cfg::strings::Main.GetString(351) + "\n";
            }
            else {
                msg += FormatPlayStatsInfo(cfg::strings::Main.GetString(338), global_stats);
                if(acc::HasSelectedUser()) {
                    const auto user_stats = sub_cnt.GetUserPlayStats(acc::GetSelectedUser());
                    msg += "\n";
                    msg += FormatPlayStatsInfo(cfg::strings::Main.GetString(337), user_stats);
                }
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

        std::vector<std::string> options;
        int remove_title_opt = -10;
        int export_opt = -10;
        int mount_save_data_opt = -10;
        int remove_tik_opt = -10;
        int reset_launch_ver_opt = -10;
        u32 cur_last_opt = 0;

        if(sub_cnt.storage_id != NcmStorageId_GameCard) {
            if(sub_cnt.storage_id != NcmStorageId_BuiltInSystem) {
                options.push_back(cfg::strings::Main.GetString(245));
                remove_title_opt = cur_last_opt;
                cur_last_opt++;
            }

            options.push_back(cfg::strings::Main.GetString(244));
            export_opt = cur_last_opt;
            cur_last_opt++;
        }

        if(sub_cnt.storage_id != NcmStorageId_BuiltInSystem) {
            if(sub_cnt.IsBaseTitle()) {
                options.push_back(cfg::strings::Main.GetString(414));
                mount_save_data_opt = cur_last_opt;
                cur_last_opt++;
            }

            if(has_tik) {
                options.push_back(cfg::strings::Main.GetString(293));
                remove_tik_opt = cur_last_opt;
                cur_last_opt++;
            }

            options.push_back(cfg::strings::Main.GetString(319));
            reset_launch_ver_opt = cur_last_opt;
            cur_last_opt++;
        }

        options.push_back(cfg::strings::Main.GetString(18));

        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), msg, options, true, icon);
        if(option_1 == remove_title_opt) {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(243), cfg::strings::Main.GetString(186), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
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
                        this->cnt_subcontents.erase(this->cnt_subcontents.begin() + idx);
                        if(this->cnt_subcontents.empty()) {
                            g_MainApplication->GetStorageContentsLayout()->Reload();
                            g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(32), "Storage", cfg::strings::Main.GetString(33));
                            g_MainApplication->LoadLayout(g_MainApplication->GetStorageContentsLayout());
                        }
                        else {
                            this->UpdateElements();
                        }
                    }
                }
                if(R_FAILED(rc)) {
                    HandleResult(rc, cfg::strings::Main.GetString(247));
                }
            }
        }
        else if(option_1 == export_opt) {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(182), cfg::strings::Main.GetString(184), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
                g_MainApplication->LoadLayout(g_MainApplication->GetTitleDumperLayout());
                g_MainApplication->GetTitleDumperLayout()->StartDump(sub_cnt, has_tik);
                g_MainApplication->LoadLayout(g_MainApplication->GetContentInformationLayout());
            }
        }
        else if(option_1 == mount_save_data_opt) {
            if(!acc::HasSelectedUser()) {
                g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(408), cfg::strings::Main.GetString(411), { cfg::strings::Main.GetString(234) }, true);
                return;
            }

            FsFileSystem save_data_fs;
            const auto rc = fs::MountTitleSaveData(sub_cnt.app_id, acc::GetSelectedUser(), save_data_fs);
            if(R_SUCCEEDED(rc)) {
                auto save_data_exp = new fs::FspExplorer(save_data_fs, "fs.SaveData-0x" + hos::FormatApplicationId(sub_cnt.app_id));
                save_data_exp->SetShouldWarnOnWriteAccess(true);
                g_MainApplication->GetExploreMenuLayout()->AddMountedExplorer(save_data_exp, cfg::strings::Main.GetString(408), icon);
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(412));
            }
            else {
                HandleResult(rc, cfg::strings::Main.GetString(413));
            }
        }
        else if(option_1 == remove_tik_opt) {
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
        else if(option_1 == reset_launch_ver_opt) {
            const auto option_2 = g_MainApplication->CreateShowDialog("Reset launch version", "Would you really like to reset this content's launch version?\nDon't do this unless you know what you're doing...", { "Yes", "Cancel" }, true);
            if(option_2 == 0) {
                const auto rc = hos::UpdateTitleVersion(sub_cnt);
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(322));
                    this->UpdateElements();
                }
                else {
                    HandleResult(rc, cfg::strings::Main.GetString(234));
                }
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
        auto head = hos::FormatApplicationId(content.app_id);
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