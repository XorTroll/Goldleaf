
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

#include <ui/ui_OwnSettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline void SaveChanges(const bool requires_reboot) {
            g_Settings.Save();

            auto notif_text = cfg::Strings.GetString(445);
            if(requires_reboot) {
                notif_text += " (" + cfg::Strings.GetString(446) + ").";
            }
            g_MainApplication->ShowNotification(notif_text);

            g_MainApplication->GetOwnSettingsLayout()->UpdateSettings(false);
        }

    }

    OwnSettingsLayout::OwnSettingsLayout() : pu::ui::Layout() {
        this->settings_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->settings_menu);
        this->UpdateSettings(true);
        this->Add(this->settings_menu);
    }

    void OwnSettingsLayout::UpdateSettings(const bool reset_selected_idx) {
        const auto old_idx = this->settings_menu->GetSelectedIndex();
        this->settings_menu->ClearItems();

        const auto custom_lang_item_name = cfg::Strings.GetString(441) + ": " + (g_Settings.has_custom_lang ? GetLanguageCode(g_Settings.custom_lang) : cfg::Strings.GetString(442));
        this->custom_lang_item = pu::ui::elm::MenuItem::New(custom_lang_item_name);
        this->custom_lang_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->custom_lang_item->SetColor(g_Settings.custom_scheme.text);
        this->custom_lang_item->AddOnKey(std::bind(&OwnSettingsLayout::custom_lang_DefaultKey, this));

        const auto use_12h_time_name = cfg::Strings.GetString(452) + ": " + (g_Settings.use_12h_time ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->use_12h_time_item = pu::ui::elm::MenuItem::New(use_12h_time_name);
        this->use_12h_time_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->use_12h_time_item->SetColor(g_Settings.custom_scheme.text);
        this->use_12h_time_item->AddOnKey(std::bind(&OwnSettingsLayout::use_12h_time_DefaultKey, this));

        const auto ignore_hidden_files_name = cfg::Strings.GetString(474) + ": " + (g_Settings.ignore_hidden_files ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->ignore_hidden_files_item = pu::ui::elm::MenuItem::New(ignore_hidden_files_name);
        this->ignore_hidden_files_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->ignore_hidden_files_item->SetColor(g_Settings.custom_scheme.text);
        this->ignore_hidden_files_item->AddOnKey(std::bind(&OwnSettingsLayout::ignore_hidden_files_DefaultKey, this));
        
        this->settings_menu->AddItem(this->custom_lang_item);
        this->settings_menu->AddItem(this->use_12h_time_item);
        this->settings_menu->AddItem(this->ignore_hidden_files_item);

        const auto ignore_required_fw_version_item_name = cfg::Strings.GetString(444) + ": " + (g_Settings.ignore_required_fw_ver ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->ignore_required_fw_version_item = pu::ui::elm::MenuItem::New(ignore_required_fw_version_item_name);
        this->ignore_required_fw_version_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->ignore_required_fw_version_item->SetColor(g_Settings.custom_scheme.text);
        this->ignore_required_fw_version_item->AddOnKey(std::bind(&OwnSettingsLayout::ignore_required_fw_version_DefaultKey, this));

        const auto show_deletion_prompt_after_install_name = cfg::Strings.GetString(475) + ": " + (g_Settings.show_deletion_prompt_after_install ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->show_deletion_prompt_after_install_item = pu::ui::elm::MenuItem::New(show_deletion_prompt_after_install_name);
        this->show_deletion_prompt_after_install_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->show_deletion_prompt_after_install_item->SetColor(g_Settings.custom_scheme.text);
        this->show_deletion_prompt_after_install_item->AddOnKey(std::bind(&OwnSettingsLayout::show_deletion_prompt_after_install_DefaultKey, this));

        this->settings_menu->AddItem(this->ignore_required_fw_version_item);
        this->settings_menu->AddItem(this->show_deletion_prompt_after_install_item);

        if(!reset_selected_idx) {
            this->settings_menu->SetSelectedIndex(old_idx);
        }
    }

    void OwnSettingsLayout::custom_lang_DefaultKey() {
        std::vector<std::string> lang_opts = { cfg::Strings.GetString(442) };
        for(u32 i = 0; i < static_cast<u32>(Language::Count); i++) {
            lang_opts.push_back(GetLanguageCode(static_cast<Language>(i)));
        }
        lang_opts.push_back(cfg::Strings.GetString(18));

        const auto option = g_MainApplication->CreateShowDialog(cfg::Strings.GetString(441), cfg::Strings.GetString(443), lang_opts, true);
        if(option >= 0) {
            if(option == 0) {
                g_Settings.has_custom_lang = false;
            }
            else {
                g_Settings.has_custom_lang = true;
                g_Settings.custom_lang = static_cast<Language>(option - 1);
            }
            SaveChanges(true);
        }
    }

    void OwnSettingsLayout::use_12h_time_DefaultKey() {
        g_Settings.use_12h_time = !g_Settings.use_12h_time;
        SaveChanges(false);
    }

    void OwnSettingsLayout::ignore_hidden_files_DefaultKey() {
        g_Settings.ignore_hidden_files = !g_Settings.ignore_hidden_files;
        SaveChanges(false);
    }

    void OwnSettingsLayout::ignore_required_fw_version_DefaultKey() {
        g_Settings.ignore_required_fw_ver = !g_Settings.ignore_required_fw_ver;
        SaveChanges(false);
    }

    void OwnSettingsLayout::show_deletion_prompt_after_install_DefaultKey() {
        g_Settings.show_deletion_prompt_after_install = !g_Settings.show_deletion_prompt_after_install;
        SaveChanges(false);
    }

}