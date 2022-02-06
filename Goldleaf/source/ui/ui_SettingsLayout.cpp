
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

#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    SettingsLayout::SettingsLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->options_menu);
        auto fw_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(352));
        fw_itm->SetColor(g_Settings.custom_scheme.text);
        fw_itm->AddOnKey(std::bind(&SettingsLayout::optsFirmware_DefaultKey, this));
        this->options_menu->AddItem(fw_itm);
        auto mem_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(353));
        mem_itm->SetColor(g_Settings.custom_scheme.text);
        mem_itm->AddOnKey(std::bind(&SettingsLayout::optsMemory_DefaultKey, this));
        this->options_menu->AddItem(mem_itm);
        auto cfg_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(354));
        cfg_itm->SetColor(g_Settings.custom_scheme.text);
        cfg_itm->AddOnKey(std::bind(&SettingsLayout::optsConfig_DefaultKey, this));
        this->options_menu->AddItem(cfg_itm);
        this->info_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->info_p_bar->SetVisible(false);
        g_Settings.ApplyProgressBarColor(this->info_p_bar);
        this->Add(this->options_menu);
        this->Add(this->info_p_bar);
    }

    void SettingsLayout::optsConfig_DefaultKey() {
        // TODO: extend config viewing/editing
        auto msg = cfg::strings::Main.GetString(354) + ":\n\n" + cfg::strings::Main.GetString(355) + ": " + cfg::strings::Main.GetString(g_Settings.ignore_required_fw_ver ? 111 : 112);
        if(!g_Settings.external_romfs.empty()) {
            auto sd_exp = fs::GetSdCardExplorer();
            auto pres_ext_romfs = sd_exp->MakeAbsolutePresentable(g_Settings.external_romfs);
            msg += "\n" + cfg::strings::Main.GetString(356) + ": \'" + pres_ext_romfs + "\'";
        }
        g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(357), msg, { cfg::strings::Main.GetString(234) }, true);
    }

    void SettingsLayout::ExportUpdateToDirectory(const std::string &fw_path, const SetSysFirmwareVersion &fw_ver) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->options_menu->SetVisible(false);
        this->info_p_bar->SetVisible(true);
        std::string fw_display_version = fw_ver.display_version;
        g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(359) + " " + fw_display_version + "...");
        const auto out_dir = sd_exp->FullPathFor(GLEAF_PATH_DUMP_UPDATE_DIR "/" + fw_display_version);
        sd_exp->DeleteDirectory(out_dir);
        exp->CopyDirectoryProgress(fw_path, out_dir, [&](const size_t cur_size, const size_t total_size, const std::string &cur_file_name, const size_t cur_file_size, const size_t total_file_size) {
            this->info_p_bar->SetMaxProgress(total_size);
            this->info_p_bar->SetProgress(cur_size);
            g_MainApplication->CallForRender();
        });
        // Rename meta NCAs to .cnmt.nca so that this update is valid for Daybreak
        const auto files = sd_exp->GetFiles(out_dir);
        for(const auto &file: files) {
            const auto original_nca = "@SystemContent:/registered/" + file;
            FsFileSystem nca_fs;
            const auto rc = fsOpenFileSystemWithId(&nca_fs, 0, FsFileSystemType_ContentMeta, original_nca.c_str());
            if(R_SUCCEEDED(rc)) {
                // Is a meta NCA
                const auto out_nca_path = out_dir + "/" + file;
                const auto out_nca_path_noext = out_nca_path.substr(0, out_nca_path.length() - 3);
                const auto out_cnmt_nca_path = out_nca_path_noext + "cnmt.nca";
                sd_exp->RenameFile(out_nca_path, out_cnmt_nca_path);
            }
        }
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
        this->options_menu->SetVisible(true);
        this->info_p_bar->SetVisible(false);
        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(358) + " '" + out_dir + "'.");
    }

    void SettingsLayout::HandleUpdate(const std::string &fw_path_base, const SetSysFirmwareVersion &fw_ver) {
        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(360), cfg::strings::Main.GetString(361), { cfg::strings::Main.GetString(377), cfg::strings::Main.GetString(18)}, true);
        if(option == 0) {
            ExportUpdateToDirectory(fw_path_base, fw_ver);
        }
    }

    void SettingsLayout::optsFirmware_DefaultKey() {
        SetSysFirmwareVersion fw_ver = {};
        setsysGetFirmwareVersion(&fw_ver);
        auto msg = cfg::strings::Main.GetString(362) + ":\n";
        msg += std::string("\n") + cfg::strings::Main.GetString(363) + ": " + fw_ver.display_version + " (" + fw_ver.display_title + ")";
        msg += std::string("\n") + cfg::strings::Main.GetString(364) + ": '" + fw_ver.version_hash + "'";
        msg += std::string("\n") + cfg::strings::Main.GetString(95) + " " + std::to_string(hos::ReadSystemKeyGeneration());
        msg += "\n\n" + cfg::strings::Main.GetString(365) + ":\n";
        hos::PendingUpdateVersion pupd = {};
        const auto is_pending_present = hos::GetPendingUpdateInfo(&pupd);
        const auto is_update_present = hos::ConvertPendingUpdateVersion(pupd);
        if(is_pending_present) {
            msg += std::string("\n") + cfg::strings::Main.GetString(363) + ": " + std::to_string(pupd.major) + "." + std::to_string(pupd.minor) + "." + std::to_string(pupd.micro);
            msg += "\n" + cfg::strings::Main.GetString(366);
        }
        else {
            msg += "\n" + cfg::strings::Main.GetString(367);
        }

        std::vector<std::string> options = { cfg::strings::Main.GetString(234), cfg::strings::Main.GetString(368) };
        if(is_pending_present) {
            options.push_back(cfg::strings::Main.GetString(369));
            options.push_back(cfg::strings::Main.GetString(370));
        }

        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(360), msg, options, false);
        if(option_1 == 0) {
            return;
        }
        else if(option_1 == 1) {
            this->HandleUpdate("Contents/registered", fw_ver);
        }
        else if(option_1 == 2)
        {
            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(371), cfg::strings::Main.GetString(372) + "\n" + cfg::strings::Main.GetString(373), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(option_2 == 0) {
                hos::CleanPendingUpdate();
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(374));
            }
        }
        else if(option_1 == 3) {
            this->HandleUpdate("Contents/placehld", is_update_present);
        }
    }

    void SettingsLayout::optsMemory_DefaultKey() {
        g_MainApplication->GetMemoryLayout()->UpdateElements();
        g_MainApplication->LoadLayout(g_MainApplication->GetMemoryLayout());
    }

}