
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

#include <ui/ui_OwnSettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline void SaveChanges(const bool requires_reboot) {
            g_Settings.Save();

            std::string notif_text = "Settings were successfully changed and saved";
            if(requires_reboot) {
                notif_text += " (changes require a reboot to show).";
            }
            g_MainApplication->ShowNotification(notif_text);

            g_MainApplication->GetOwnSettingsLayout()->UpdateSettings();
        }

    }

    OwnSettingsLayout::OwnSettingsLayout() : pu::ui::Layout() {
        this->settings_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->settings_menu);
        this->UpdateSettings();
        this->Add(this->settings_menu);
    }

    void OwnSettingsLayout::UpdateSettings() {
        this->settings_menu->ClearItems();

        auto custom_lang_item_name = "Custom language: " + (g_Settings.has_custom_lang ? LanguageToString(g_Settings.custom_lang) : "none");
        this->custom_lang_item = pu::ui::elm::MenuItem::New(custom_lang_item_name);
        this->custom_lang_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->custom_lang_item->SetColor(g_Settings.custom_scheme.text);
        this->custom_lang_item->AddOnKey(std::bind(&OwnSettingsLayout::custom_lang_DefaultKey, this));

        auto ignore_required_fw_version_item_name = std::string("Ignore required firmware version on installs: ") + (g_Settings.ignore_required_fw_ver ? "true" : "false");
        this->ignore_required_fw_version_item = pu::ui::elm::MenuItem::New(ignore_required_fw_version_item_name);
        this->ignore_required_fw_version_item->SetIcon(g_Settings.PathForResource("/Common/Settings.png"));
        this->ignore_required_fw_version_item->SetColor(g_Settings.custom_scheme.text);
        this->ignore_required_fw_version_item->AddOnKey(std::bind(&OwnSettingsLayout::ignore_required_fw_version_DefaultKey, this));
        
        this->settings_menu->AddItem(this->custom_lang_item);
        this->settings_menu->AddItem(this->ignore_required_fw_version_item);

        this->settings_menu->SetSelectedIndex(0);
    }

    void OwnSettingsLayout::custom_lang_DefaultKey() {
        std::vector<std::string> lang_opts = { "None" };
        for(u32 i = 0; i < static_cast<u32>(Language::Count); i++) {
            lang_opts.push_back(LanguageToString(static_cast<Language>(i)));
        }
        lang_opts.push_back("Cancel");

        const auto option = g_MainApplication->CreateShowDialog("Custom language", "Select the custom language", lang_opts, true);
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

    void OwnSettingsLayout::ignore_required_fw_version_DefaultKey() {
        g_Settings.ignore_required_fw_ver = !g_Settings.ignore_required_fw_ver;
        SaveChanges(false);
    }

}