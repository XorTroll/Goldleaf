
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#include <ui/ui_ApplicationContentsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline std::string FormatApplicationPlayStats(const std::string &kind, const cnt::ApplicationPlayStats &stats) {
            std::string fmt;
            fmt += "\n" + kind;
            fmt += "\n" + cfg::Strings.GetString(339) + " " + util::FormatTime(stats.secs_from_last_launched);
            fmt += "\n" + cfg::Strings.GetString(340) + " " + util::FormatTime(stats.total_play_secs);
            return fmt;
        }

    }

    void ApplicationContentsLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    ApplicationContentsLayout::ApplicationContentsLayout() {
        this->cnts_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->cnts_menu);
        this->Add(this->cnts_menu);

        this->SetOnInput(std::bind(&ApplicationContentsLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void ApplicationContentsLayout::DisplayContentInfo(const u32 cnt_idx) {
        std::string info;

        const auto &app = cnt::GetApplications().at(this->app_idx);
        const auto &cnt_status = app.meta_status_list.at(cnt_idx);

        info += cfg::Strings.GetString(503) + ": " + cnt::GetContentMetaTypeName(static_cast<NcmContentMetaType>(cnt_status.meta_type)) + "\n";
        info += cfg::Strings.GetString(504) + ": " + cnt::GetStorageIdName(static_cast<NcmStorageId>(cnt_status.storageID)) + "\n";

        switch(cnt_status.meta_type) {
            case NcmContentMetaType_AddOnContent: {
                info += cfg::Strings.GetString(490) + " " + util::FormatApplicationId(cnt_status.application_id) + "\n";
                info += cfg::Strings.GetString(263) + " " + std::to_string(cnt::GetAddOnContentId(cnt_status.application_id)) + "\n";
                break;
            }
            case NcmContentMetaType_Patch: {
                info += cfg::Strings.GetString(491) + " " + util::FormatApplicationId(cnt_status.application_id) + "\n";
                break;
            }
            case NcmContentMetaType_Application: {
                info += cfg::Strings.GetString(90) + " " + util::FormatApplicationId(cnt_status.application_id) + "\n";
                break;
            }
        }

        info += cfg::Strings.GetString(178) + " " + std::to_string(cnt_status.version) + "\n";
        info += cfg::Strings.GetString(93);

        bool any_content = false;
        for(u32 i = 0; i < cnt::MaxContentCount; i++) {
            if(app.contents.at(cnt_idx).cnt_ids[i].has_value()) {
                info += "\n";
                const auto is_meta = (i == static_cast<u32>(NcmContentType_Meta));
                info += "    " + cnt::GetContentTypeName(static_cast<NcmContentType>(i)) + ": " + util::FormatContentId(app.contents.at(cnt_idx).cnt_ids[i].value()) + (is_meta ? ".cnmt.nca" : ".nca");
                any_content = true;
            }
        }
        if(!any_content) {
            info += " " + cfg::Strings.GetString(442);
        }
        info += "\n\n";

        cnt::Ticket tik;
        const auto has_tik = cnt::TryFindApplicationTicket(cnt_status.application_id, tik);
        if(has_tik) {
            const auto key_gen = esGetRightsIdKeyGeneration(&tik.rights_id);
            info += cfg::Strings.GetString(201) + " " + std::string((tik.type == cnt::TicketType::Common) ? cfg::Strings.GetString(448) : cfg::Strings.GetString(449)) + "\n";
            info += cfg::Strings.GetString(95) + " " + std::to_string(key_gen) + " (" + hos::GetKeyGenerationRange(key_gen) + ")\n";
            info += "\n";
        }

        std::vector<std::string> options = { cfg::Strings.GetString(234) };
        const auto has_contents = (cnt_status.storageID == NcmStorageId_SdCard) || (cnt_status.storageID == NcmStorageId_BuiltInUser) || (cnt_status.storageID == NcmStorageId_GameCard);
        if(has_contents) {
            options.push_back(cfg::Strings.GetString(245));
            options.push_back(cfg::Strings.GetString(244));
        }
        if(has_tik) {
            options.push_back(cfg::Strings.GetString(293));
        }

        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(243), info, options, false);
        if(option == 1) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(243), cfg::Strings.GetString(186), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                auto remove_tik = false;
                if(has_tik) {
                    const auto option_3 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(243), cfg::Strings.GetString(204), { cfg::Strings.GetString(111), cfg::Strings.GetString(112), cfg::Strings.GetString(18) }, true);
                    remove_tik = option_3 == 0;
                }

                const auto app_id = app.record.id;
                const auto cnt_count = app.contents.size();

                cnt::RemoveApplicationContentById(app, cnt_idx);
                if(remove_tik) {
                    cnt::RemoveTicket(tik);
                }

                if(cnt_count == 1) {
                    g_MainApplication->GetApplicationListLayout()->ReloadApplications();
                    g_MainApplication->ReturnToParentLayout();
                }
                else {
                    this->LoadApplication(this->app_idx);

                    const auto &new_app = cnt::GetApplications().at(this->app_idx);
                    if(new_app.record.id != app_id) {
                        // If application ordering changed, better to reload the list
                        g_MainApplication->GetApplicationListLayout()->ReloadApplications();
                        g_MainApplication->ReturnToParentLayout();
                    }
                }
            }
        }
        else if(option == 2) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(182), cfg::Strings.GetString(184), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                g_MainApplication->ShowLayout(g_MainApplication->GetContentExportLayout());
                g_MainApplication->GetContentExportLayout()->StartExport(app, cnt_idx, has_tik);
            }
        }
        else if(option == 3) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(200), cfg::Strings.GetString(205), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                const auto rc = esDeleteTicket(&tik.rights_id);
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(206));
                    cnt::NotifyTicketsChanged();
                    this->LoadApplication(this->app_idx);
                }
                else {
                    HandleResult(rc, cfg::Strings.GetString(207));
                }
            }
        }
    }

    void ApplicationContentsLayout::DisplayApplicationInfo() {
        std::string info;

        const auto &app = cnt::GetApplications().at(this->app_idx);
        info += cfg::Strings.GetString(90) + " " + util::FormatApplicationId(app.record.id) + "\n";
        info += cfg::Strings.GetString(492) + " " + app.cache.record_last_event + "\n";
        info += cfg::Strings.GetString(493);
        for(const auto &flag: app.cache.view_flags) {
            info += "\n\uE090 " + flag;
        }
        if(app.cache.view_flags.empty()) {
            info += " " + cfg::Strings.GetString(442);
        }
        info += "\n";

        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(162), info, { cfg::Strings.GetString(234), cfg::Strings.GetString(494), cfg::Strings.GetString(495), cfg::Strings.GetString(414), cfg::Strings.GetString(319), cfg::Strings.GetString(245) }, false, GetApplicationIcon(app.record.id));
        if(option == 1) {
            std::string size_info;

            size_t total_app_size = 0;
            size_t total_patch_size = 0;
            size_t total_aoc_size = 0;
            for(u32 i = 0; i < 4; i++) {
                const auto storage_id = static_cast<NcmStorageId>(app.occupied_size.entities[i].storage_id);
                if(storage_id == NcmStorageId_None) {
                    continue;
                }

                total_app_size += app.occupied_size.entities[i].app_size;
                total_patch_size += app.occupied_size.entities[i].patch_size;
                total_aoc_size += app.occupied_size.entities[i].add_on_content_size;

                size_info += cnt::GetStorageIdName(static_cast<NcmStorageId>(app.occupied_size.entities[i].storage_id)) + ":\n";
                size_info += "\uE090 " + cfg::Strings.GetString(261) + ": " + fs::FormatSize(app.occupied_size.entities[i].app_size) + "\n";
                size_info += "\uE090 " + cfg::Strings.GetString(262) + ": " + fs::FormatSize(app.occupied_size.entities[i].patch_size) + "\n";
                size_info += "\uE090 " + cfg::Strings.GetString(263) + ": " + fs::FormatSize(app.occupied_size.entities[i].add_on_content_size) + "\n";
                size_info += "\uE090 " + cfg::Strings.GetString(496) + ": " + fs::FormatSize(app.occupied_size.entities[i].app_size + app.occupied_size.entities[i].patch_size + app.occupied_size.entities[i].add_on_content_size) + "\n";
                size_info += "\n";
            }

            size_info += cfg::Strings.GetString(496) + ":\n";
            size_info += "\uE090 " + cfg::Strings.GetString(261) + ": " + fs::FormatSize(total_app_size) + "\n";
            size_info += "\uE090 " + cfg::Strings.GetString(262) + ": " + fs::FormatSize(total_patch_size) + "\n";
            size_info += "\uE090 " + cfg::Strings.GetString(263) + ": " + fs::FormatSize(total_aoc_size) + "\n";
            size_info += "\uE090 " + cfg::Strings.GetString(496) + ": " + fs::FormatSize(total_app_size + total_patch_size + total_aoc_size) + "\n";

            g_MainApplication->DisplayDialog(cfg::Strings.GetString(497), size_info, { cfg::Strings.GetString(234) }, true, GetApplicationIcon(app.record.id));
        }
        else if(option == 2) {
            std::string play_info;

            const auto global_stats = app.GetGlobalPlayStats();
            if(global_stats.total_play_secs == 0) {
                play_info += "\n" + cfg::Strings.GetString(351) + "\n";
            }
            else {
                play_info += FormatApplicationPlayStats(cfg::Strings.GetString(338), global_stats);

                play_info += "\n";

                if(acc::HasSelectedUser()) {
                    const auto user_stats = app.GetUserPlayStats(acc::GetSelectedUser());
                    play_info += FormatApplicationPlayStats(cfg::Strings.GetString(337), user_stats);
                }
                else {
                    play_info += "\n" + cfg::Strings.GetString(498) + "\n";
                }
            }

            g_MainApplication->DisplayDialog(cfg::Strings.GetString(499), play_info, { cfg::Strings.GetString(234) }, true, GetApplicationIcon(app.record.id));
        }
        else if(option == 3) {
            Result rc;
            FsFileSystem save_data_fs;
            if(app.misc_data.device_save_data_size > 0) {
                // Game uses device save data
                rc = fs::MountDeviceSaveData(app.record.id, save_data_fs);
            }
            else if(app.misc_data.user_account_save_data_size > 0) {
                if(!acc::HasSelectedUser()) {
                    g_MainApplication->DisplayDialog(cfg::Strings.GetString(408), cfg::Strings.GetString(411), { cfg::Strings.GetString(234) }, true);
                    g_MainApplication->PickUser();
                    return;
                }

                // Game uses account save data
                rc = fs::MountAccountSaveData(app.record.id, acc::GetSelectedUser(), save_data_fs);
            }
            else {
                g_MainApplication->ShowNotification(cfg::Strings.GetString(500));
                return;
            }

            if(R_SUCCEEDED(rc)) {
                auto save_data_exp = new fs::FspExplorer(save_data_fs, "fs.SaveData-0x" + util::FormatApplicationId(app.record.id));
                save_data_exp->SetShouldWarnOnWriteAccess(true);
                g_MainApplication->GetExploreMenuLayout()->AddMountedExplorer(save_data_exp, cfg::Strings.GetString(408), GetApplicationIcon(app.record.id));
                g_MainApplication->ShowNotification(cfg::Strings.GetString(412));
            }
            else {
                HandleResult(rc, cfg::Strings.GetString(413));
            }
        }
        else if(option == 4) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(319), cfg::Strings.GetString(457), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                const auto rc = cnt::UpdateApplicationVersion(app);
                if(R_SUCCEEDED(rc)) {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(322));
                }
                else {
                    HandleResult(rc, cfg::Strings.GetString(234));
                }
            }
        }
        else if(option == 5) {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(243), cfg::Strings.GetString(186), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                // TODO: also delete all tickets?
                cnt::RemoveApplicationById(app.record.id);
                g_MainApplication->GetApplicationListLayout()->ReloadApplications();
                g_MainApplication->ReturnToParentLayout();
            }
        }
    }

    void ApplicationContentsLayout::LoadApplication(const u32 app_i) {
        this->cnts_menu->ClearItems();
        this->app_idx = app_i;

        auto app_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(501));
        app_itm->SetColor(g_Settings.GetColorScheme().text);
        app_itm->AddOnKey(std::bind(&ApplicationContentsLayout::DisplayApplicationInfo, this));
        this->cnts_menu->AddItem(app_itm);

        const auto &app = cnt::GetApplications().at(app_i);
        u32 cnt_i = 0;
        for(const auto &cnt_status: app.meta_status_list) {
            std::string name;
            if(cnt_status.meta_type == NcmContentMetaType_AddOnContent) {
                const auto aoc_id = cnt::GetAddOnContentId(cnt_status.application_id);
                name = cfg::Strings.GetString(263) + " " + std::to_string(aoc_id);
            }
            else if(cnt_status.meta_type == NcmContentMetaType_Patch) {
                name = cfg::Strings.GetString(262);
            }
            else if(cnt_status.meta_type == NcmContentMetaType_Application) {
                name = cfg::Strings.GetString(261);
            }
            else {
                name = cfg::Strings.GetString(502);
            }

            auto itm = pu::ui::elm::MenuItem::New(name);
            itm->SetColor(g_Settings.GetColorScheme().text);
            itm->AddOnKey(std::bind(&ApplicationContentsLayout::DisplayContentInfo, this, cnt_i));
            this->cnts_menu->AddItem(itm);
            cnt_i++;
        }

        g_MainApplication->LoadMenuData(true, app.cache.display_name, GetApplicationIcon(app.record.id), app.cache.display_author + ", v" + app.misc_data.display_version);
    }

}
