
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

#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;
extern bool g_UpdatedNeedsRename;

namespace ui {

    UpdateLayout::UpdateLayout() {
        this->info_text = pu::ui::elm::TextBlock::New(150, 320, "(...)");
        this->info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->info_text->SetColor(g_Settings.custom_scheme.text);
        this->download_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->download_p_bar);
        this->Add(this->info_text);
        this->Add(this->download_p_bar);
    }

    void UpdateLayout::StartUpdateSearch() {
        this->download_p_bar->SetVisible(false);
        this->info_text->SetText(cfg::strings::Main.GetString(305));
        g_MainApplication->CallForRender();

        const auto json_data = net::RetrieveContent("https://api.github.com/repos/XorTroll/Goldleaf/releases", "application/json");
        if(json_data.empty()) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(316), { cfg::strings::Main.GetString(234) }, true);
            g_MainApplication->ReturnToMainMenu();
            return;
        }
        const auto json = JSON::parse(json_data);
        if(json.size() <= 0) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(316), { cfg::strings::Main.GetString(234) }, true);
            g_MainApplication->ReturnToMainMenu();
            return;
        }
        const auto last_id = json[0].value("tag_name", "");
        if(last_id.empty()) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(316), { cfg::strings::Main.GetString(234) }, true);
            g_MainApplication->ReturnToMainMenu();
            return;
        }
        this->info_text->SetText(cfg::strings::Main.GetString(306));
        g_MainApplication->CallForRender();

        const auto last_ver = Version::FromString(last_id);
        const auto cur_ver = Version::MakeVersion(GOLDLEAF_MAJOR, GOLDLEAF_MINOR, GOLDLEAF_MICRO);
        if(last_ver.IsEqual(cur_ver)) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(307), { cfg::strings::Main.GetString(234) }, true);
        }
        else if(last_ver.IsLower(cur_ver)) {
            const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(308), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option == 0) {
                EnsureDirectories();
                auto sd_exp = fs::GetSdCardExplorer();
                sd_exp->DeleteFile(GLEAF_PATH_TEMP_UPDATE_NRO);

                this->info_text->SetText(cfg::strings::Main.GetString(309));
                g_MainApplication->CallForRender();
                
                hos::LockAutoSleep();
                this->download_p_bar->SetVisible(true);
                const auto download_url = "https://github.com/XorTroll/Goldleaf/releases/download/" + last_id + "/Goldleaf.nro";
                sd_exp->DeleteFile(GLEAF_PATH_TEMP_UPDATE_NRO);
                net::RetrieveToFile(download_url, sd_exp->AbsolutePathFor(GLEAF_PATH_TEMP_UPDATE_NRO), [&](const double done, const double total) {
                    this->download_p_bar->SetMaxProgress(total);
                    this->download_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->download_p_bar->SetVisible(false);
                hos::UnlockAutoSleep();

                if(sd_exp->IsFile(GLEAF_PATH_TEMP_UPDATE_NRO)) {
                    g_UpdatedNeedsRename = true;
                }
                
                g_MainApplication->CallForRender();
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(314) + " " + cfg::strings::Main.GetString(315));
            }
        }
        else if(last_ver.IsHigher(cur_ver)) {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(316), { cfg::strings::Main.GetString(234) }, true);
        }
        g_MainApplication->ReturnToMainMenu();
    }

}