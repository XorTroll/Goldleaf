
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

#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <upd/upd_Pending.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void SettingsLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    SettingsLayout::SettingsLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->options_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        auto fw_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(352));
        fw_itm->SetColor(g_Settings.GetColorScheme().text);
        fw_itm->AddOnKey(std::bind(&SettingsLayout::optsFirmware_DefaultKey, this));
        this->options_menu->AddItem(fw_itm);
        auto mem_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(353));
        mem_itm->SetColor(g_Settings.GetColorScheme().text);
        mem_itm->AddOnKey(std::bind(&SettingsLayout::optsMemory_DefaultKey, this));
        this->options_menu->AddItem(mem_itm);
        auto cfg_itm = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(354));
        cfg_itm->SetColor(g_Settings.GetColorScheme().text);
        cfg_itm->AddOnKey(std::bind(&SettingsLayout::optsConfig_DefaultKey, this));
        this->options_menu->AddItem(cfg_itm);
        const s32 p_bar_x = 120;
        this->fw_export_p_bar = pu::ui::elm::ProgressBar::New(p_bar_x, 300, pu::ui::render::ScreenWidth - (2 * p_bar_x), 30, 100.0f);
        this->fw_export_p_bar->SetVisible(false);
        this->fw_export_p_bar->SetProgressColor(g_Settings.GetColorScheme().progress_bar);
        this->fw_export_p_bar->SetBackgroundColor(g_Settings.GetColorScheme().progress_bar_bg);
        this->Add(this->options_menu);
        this->Add(this->fw_export_p_bar);

        this->SetOnInput(std::bind(&SettingsLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void SettingsLayout::optsConfig_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, "Goldleaf settings", CommonIconKind::Settings, "Goldleaf-specific settings");
        g_MainApplication->GetOwnSettingsLayout()->UpdateSettings(true);
        g_MainApplication->ShowLayout(g_MainApplication->GetOwnSettingsLayout());
    }

    void SettingsLayout::ExportUpdateToDirectory(const std::string &fw_path, const SetSysFirmwareVersion &fw_ver) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->options_menu->SetVisible(false);
        this->fw_export_p_bar->SetVisible(true);
        std::string fw_display_version = fw_ver.display_version;
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(43), CommonIconKind::Settings, cfg::Strings.GetString(359) + " " + fw_display_version + "...");
        const auto out_dir = sd_exp->FullPathFor(GLEAF_PATH_EXPORT_UPDATE_DIR "/" + fw_display_version);
        sd_exp->DeleteDirectory(out_dir);
        exp->CopyDirectoryProgress(fw_path, out_dir, [&](const size_t total_size) {
            this->fw_export_p_bar->SetMaxProgress(total_size);
        }, [](const size_t file_size, const std::string &old_file, const std::string &new_file) {
            // ...
        }, [&](const size_t cur_rw_size) {
            this->fw_export_p_bar->IncrementProgress(cur_rw_size);
            g_MainApplication->CallForRender();
        });
        // Rename meta NCAs to .cnmt.nca so that this update is valid for Daybreak
        const auto files = sd_exp->GetFiles(out_dir);
        for(const auto &file: files) {
            const auto original_nca = "@SystemContent://registered/" + file;
            FsFileSystem nca_fs;
            const auto rc = fsOpenFileSystemWithId(&nca_fs, 0, FsFileSystemType_ContentMeta, original_nca.c_str(), FsContentAttributes_All);
            if(R_SUCCEEDED(rc)) {
                // Is a meta NCA
                const auto out_nca_path = out_dir + "/" + file;
                const auto out_nca_path_noext = out_nca_path.substr(0, out_nca_path.length() - __builtin_strlen("nca"));
                const auto out_cnmt_nca_path = out_nca_path_noext + "cnmt.nca";
                sd_exp->RenameFile(out_nca_path, out_cnmt_nca_path);
            }
        }
        g_MainApplication->PopMenuData();
        this->options_menu->SetVisible(true);
        this->fw_export_p_bar->SetVisible(false);
        g_MainApplication->ShowNotification(cfg::Strings.GetString(358) + " '" + out_dir + "'.");
    }

    void SettingsLayout::HandleUpdate(const std::string &fw_path_base, const SetSysFirmwareVersion &fw_ver) {
        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(360), cfg::Strings.GetString(361), { cfg::Strings.GetString(377), cfg::Strings.GetString(18)}, true);
        if(option == 0) {
            ExportUpdateToDirectory(fw_path_base, fw_ver);
        }
    }

    void SettingsLayout::optsFirmware_DefaultKey() {
        SetSysFirmwareVersion fw_ver = {};
        setsysGetFirmwareVersion(&fw_ver);
        auto msg = cfg::Strings.GetString(362) + ":\n";
        msg += std::string("\n") + cfg::Strings.GetString(363) + ": " + fw_ver.display_version + " (" + fw_ver.display_title + ")";
        msg += std::string("\n") + cfg::Strings.GetString(364) + ": '" + fw_ver.version_hash + "'";
        msg += std::string("\n") + cfg::Strings.GetString(95) + " " + std::to_string(hos::ReadSystemKeyGeneration());
        msg += "\n\n" + cfg::Strings.GetString(365) + ":\n";
        upd::PendingUpdateVersion pupd = {};
        const auto is_pending_present = upd::GetPendingUpdateInfo(&pupd);
        const auto is_update_present = upd::ConvertPendingUpdateVersion(pupd);
        if(is_pending_present) {
            msg += std::string("\n") + cfg::Strings.GetString(363) + ": " + std::to_string(pupd.major) + "." + std::to_string(pupd.minor) + "." + std::to_string(pupd.micro);
            msg += "\n" + cfg::Strings.GetString(366);
        }
        else {
            msg += "\n" + cfg::Strings.GetString(367);
        }

        std::vector<std::string> options = { cfg::Strings.GetString(234), cfg::Strings.GetString(368) };
        if(is_pending_present) {
            options.push_back(cfg::Strings.GetString(369));
            options.push_back(cfg::Strings.GetString(370));
        }

        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(360), msg, options, false);
        if(option_1 == 0) {
            return;
        }
        else if(option_1 == 1) {
            this->HandleUpdate("Contents/registered", fw_ver);
        }
        else if(option_1 == 2)
        {
            const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(371), cfg::Strings.GetString(372) + "\n" + cfg::Strings.GetString(373), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option_2 == 0) {
                upd::CleanPendingUpdate();
                g_MainApplication->ShowNotification(cfg::Strings.GetString(374));
            }
        }
        else if(option_1 == 3) {
            this->HandleUpdate("Contents/placehld", is_update_present);
        }
    }

    void SettingsLayout::optsMemory_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(353), CommonIconKind::NAND, cfg::Strings.GetString(511));
        g_MainApplication->GetMemoryLayout()->UpdateElements();
        g_MainApplication->ShowLayout(g_MainApplication->GetMemoryLayout());
    }

    void SettingsLayout::Load() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(375), CommonIconKind::Settings, cfg::Strings.GetString(376));
        this->options_menu->SetSelectedIndex(0);
    }

}
