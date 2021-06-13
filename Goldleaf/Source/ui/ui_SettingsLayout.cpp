
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

#include <ui/ui_SettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui
{
    SettingsLayout::SettingsLayout() : pu::ui::Layout() {
        this->options_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, (560 / g_Settings.menu_item_size));
        this->options_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->options_menu);
        auto itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(352));
        itm->SetColor(g_Settings.custom_scheme.Text);
        itm->AddOnClick(std::bind(&SettingsLayout::optsFirmware_Click, this));
        this->options_menu->AddItem(itm);
        auto itm2 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(353));
        itm2->SetColor(g_Settings.custom_scheme.Text);
        itm2->AddOnClick(std::bind(&SettingsLayout::optsMemory_Click, this));
        this->options_menu->AddItem(itm2);
        auto itm3 = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(354));
        itm3->SetColor(g_Settings.custom_scheme.Text);
        itm3->AddOnClick(std::bind(&SettingsLayout::optsConfig_Click, this));
        this->options_menu->AddItem(itm3);
        this->info_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->info_p_bar->SetVisible(false);
        g_Settings.ApplyProgressBarColor(this->info_p_bar);
        this->Add(this->options_menu);
        this->Add(this->info_p_bar);
    }

    void SettingsLayout::optsConfig_Click() {
        auto msg = cfg::strings::Main.GetString(354) + ":\n\n" + cfg::strings::Main.GetString(355) + ": " + cfg::strings::Main.GetString(g_Settings.ignore_required_fw_ver ? 111 : 112);
        if(!g_Settings.external_romfs.empty()) {
            auto sd_exp = fs::GetSdCardExplorer();
            auto pres_ext_romfs = sd_exp->MakeAbsolutePresentable(g_Settings.external_romfs);
            msg += "\n" + cfg::strings::Main.GetString(356) + ": \'" + pres_ext_romfs + "\'";
        }
        g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(357), msg, { cfg::strings::Main.GetString(234) }, true);
    }

    void SettingsLayout::ExportUpdateToDirectory(String fw_path, const SetSysFirmwareVersion &fw_ver) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto exp = fs::GetNANDSystemExplorer();
        this->options_menu->SetVisible(false);
        this->info_p_bar->SetVisible(true);
        g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(359) + " " + fw_ver.display_version + "...");
        auto out_dir = sd_exp->FullPathFor(consts::DumpUpdate + "/" + fw_ver.display_version);
        sd_exp->DeleteDirectory(out_dir);
        exp->CopyDirectoryProgress(fw_path, out_dir, [&](double done, double total) {
            this->info_p_bar->SetMaxValue(total);
            this->info_p_bar->SetProgress(done);
            g_MainApplication->CallForRender();
        });
        // Rename meta NCAs to .cnmt.nca so that this update is valid for Daybreak
        auto files = sd_exp->GetFiles(out_dir);
        for(auto &file: files) {
            auto original_nca = "@SystemContent:/registered/" + file;
            FsFileSystem nca_fs;
            const auto rc = fsOpenFileSystemWithId(&nca_fs, 0, FsFileSystemType_ContentMeta, original_nca.AsUTF8().c_str());
            if(R_SUCCEEDED(rc)) {
                // Is a meta NCA
                auto out_nca_path = out_dir + "/" + file;
                auto out_nca_path_noext = out_nca_path.substr(0, out_nca_path.length() - 3);
                auto out_cnmt_nca_path = out_nca_path_noext + "cnmt.nca";
                sd_exp->RenameFile(out_nca_path, out_cnmt_nca_path);
            }
        }
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(43), "Settings", cfg::strings::Main.GetString(44));
        this->options_menu->SetVisible(true);
        this->info_p_bar->SetVisible(false);
        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(358) + " '" + out_dir + "'.");
    }

    void SettingsLayout::HandleUpdate(String fw_path_base, const SetSysFirmwareVersion &fw_ver) {
        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(360), cfg::strings::Main.GetString(361), { cfg::strings::Main.GetString(377), cfg::strings::Main.GetString(18)}, true);
        if(option == 0) {
            ExportUpdateToDirectory(fw_path_base, fw_ver);
        }
    }

    void SettingsLayout::optsFirmware_Click() {
        SetSysFirmwareVersion fw_ver = {};
        setsysGetFirmwareVersion(&fw_ver);
        auto msg = cfg::strings::Main.GetString(362) + ":\n";
        msg += String("\n" + cfg::strings::Main.GetString(363) + ": ") + fw_ver.display_version + " (" + fw_ver.display_title + ")";
        msg += String("\n" + cfg::strings::Main.GetString(364) + ": '") + fw_ver.version_hash + "'";
        msg += String("\n" + cfg::strings::Main.GetString(95) + " ") + std::to_string(hos::ComputeSystemKeyGeneration());
        msg += "\n\n" + cfg::strings::Main.GetString(365) + ":\n";
        hos::PendingUpdateVersion pupd = {};
        const auto is_pending_present = hos::GetPendingUpdateInfo(&pupd);
        const auto is_update_present = hos::ConvertPendingUpdateVersion(pupd);
        if(is_pending_present) {
            msg += String("\n" + cfg::strings::Main.GetString(363) + ": ") + std::to_string(pupd.major) + "." + std::to_string(pupd.minor) + "." + std::to_string(pupd.micro);
            msg += "\n" + cfg::strings::Main.GetString(366);
        }
        else {
            msg += "\n" + cfg::strings::Main.GetString(367);
        }

        std::vector<String> options = { cfg::strings::Main.GetString(234), cfg::strings::Main.GetString(368) };
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

    void SettingsLayout::optsMemory_Click() {
        g_MainApplication->GetMemoryLayout()->UpdateElements();
        g_MainApplication->LoadLayout(g_MainApplication->GetMemoryLayout());
    }

}