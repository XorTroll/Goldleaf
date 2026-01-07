
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

#include <ui/ui_OwnSettingsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline bool FormatMenuStickMoveSpeed(const cfg::MenuStickMoveSpeed speed, std::string &out_str) {
            switch(speed) {
                case cfg::MenuStickMoveSpeed::VerySlow:
                    out_str = cfg::Strings.GetString(523);
                    return true;
                case cfg::MenuStickMoveSpeed::Slow:
                    out_str = cfg::Strings.GetString(524);
                    return true;
                case cfg::MenuStickMoveSpeed::Medium:
                    out_str = cfg::Strings.GetString(525);
                    return true;
                case cfg::MenuStickMoveSpeed::Fast:
                    out_str = cfg::Strings.GetString(526);
                    return true;
                case cfg::MenuStickMoveSpeed::VeryFast:
                    out_str = cfg::Strings.GetString(527);
                    return true;
                default:
                    return false;
            }
        }

        inline bool FormatUsbSpeed(const UsbDeviceSpeed speed, std::string &out_str) {
            switch(speed) {
                case UsbDeviceSpeed_None:
                    out_str = cfg::Strings.GetString(537);
                    return true;
                case UsbDeviceSpeed_Low:
                    out_str = cfg::Strings.GetString(538);
                    return true;
                case UsbDeviceSpeed_Full:
                    out_str = cfg::Strings.GetString(539);
                    return true;
                case UsbDeviceSpeed_High:
                    out_str = cfg::Strings.GetString(540);
                    return true;
                case UsbDeviceSpeed_Super:
                    out_str = cfg::Strings.GetString(541);
                    return true;
                default:
                    return false;
            }
        }

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

    void OwnSettingsLayout::view_logs_DefaultKey() {
        auto sd_exp = fs::GetSdCardExplorer();
        g_MainApplication->ShowLayout(g_MainApplication->GetFileContentLayout());
        g_MainApplication->GetFileContentLayout()->LoadFile(GLEAF_PATH_LOG_FILE, sd_exp->FullPresentablePathFor(GLEAF_PATH_LOG_FILE), sd_exp, false);
    }

    void OwnSettingsLayout::clear_logs_DefaultKey() {
        auto sd_exp = fs::GetSdCardExplorer();
        sd_exp->DeleteFile(GLEAF_PATH_LOG_FILE);
        g_MainApplication->ShowNotification(cfg::Strings.GetString(515));
    }

    void OwnSettingsLayout::custom_lang_DefaultKey() {
        std::vector<std::string> lang_opts = { cfg::Strings.GetString(521) };
        for(u32 i = 0; i < static_cast<u32>(Language::Count); i++) {
            lang_opts.push_back(LanguageNameList[i]);
        }
        lang_opts.push_back(cfg::Strings.GetString(18));

        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(441), cfg::Strings.GetString(443), lang_opts, true);
        if(option >= 0) {
            if(option == 0) {
                g_Settings.lang = Language::Auto;
            }
            else {
                g_Settings.lang = static_cast<Language>(option - 1);
            }
            SaveChanges(true);
        }
    }

    void OwnSettingsLayout::use_12h_time_DefaultKey() {
        g_Settings.json_settings.general.value().use_12h_time.value() = !g_Settings.json_settings.general.value().use_12h_time.value();
        SaveChanges(false);
    }

    void OwnSettingsLayout::ignore_hidden_files_DefaultKey() {
        g_Settings.json_settings.fs.value().ignore_hidden_files.value() = !g_Settings.json_settings.fs.value().ignore_hidden_files.value();
        SaveChanges(false);
    }

    void OwnSettingsLayout::compute_directory_sizes_DefaultKey() {
        g_Settings.json_settings.fs.value().compute_directory_sizes.value() = !g_Settings.json_settings.fs.value().compute_directory_sizes.value();
        SaveChanges(false);
    }

    void OwnSettingsLayout::ignore_required_fw_version_DefaultKey() {
        g_Settings.json_settings.installs.value().ignore_required_fw_version.value() = !g_Settings.json_settings.installs.value().ignore_required_fw_version.value();
        SaveChanges(false);
    }

    void OwnSettingsLayout::show_deletion_prompt_after_install_DefaultKey() {
        g_Settings.json_settings.installs.value().show_deletion_prompt_after_install.value() = !g_Settings.json_settings.installs.value().show_deletion_prompt_after_install.value();
        SaveChanges(false);
    }

    void OwnSettingsLayout::menu_stick_move_speed_DefaultKey() {
        std::vector<std::string> speed_opts;
        for(u32 i = 0; i < static_cast<u32>(cfg::MenuStickMoveSpeed::Count); i++) {
            std::string speed_str;
            GLEAF_ASSERT_TRUE(FormatMenuStickMoveSpeed(static_cast<cfg::MenuStickMoveSpeed>(i), speed_str));
            speed_opts.push_back(speed_str);
        }
        speed_opts.push_back(cfg::Strings.GetString(18));
        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(522), cfg::Strings.GetString(528), speed_opts, false);
        if(option >= 0) {
            g_Settings.json_settings.ui.value().menu_stick_move_speed.value() = static_cast<u32>(option);
            SaveChanges(false);
        }
    }

    void OwnSettingsLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    OwnSettingsLayout::OwnSettingsLayout() : pu::ui::Layout() {
        this->settings_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->settings_menu);
        this->UpdateSettings(true);
        this->Add(this->settings_menu);

        this->SetOnInput(std::bind(&OwnSettingsLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void OwnSettingsLayout::UpdateSettings(const bool reset_selected_idx) {
        g_Settings.ApplyToMenu(this->settings_menu);
        const auto old_idx = this->settings_menu->GetSelectedIndex();
        this->settings_menu->ClearItems();

        const auto view_logs_item_name = cfg::Strings.GetString(509);
        this->view_logs_item = pu::ui::elm::MenuItem::New(view_logs_item_name);
        this->view_logs_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->view_logs_item->SetColor(g_Settings.GetColorScheme().text);
        this->view_logs_item->AddOnKey(std::bind(&OwnSettingsLayout::view_logs_DefaultKey, this));

        const auto clear_logs_item_name = cfg::Strings.GetString(514);
        this->clear_logs_item = pu::ui::elm::MenuItem::New(clear_logs_item_name);
        this->clear_logs_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->clear_logs_item->SetColor(g_Settings.GetColorScheme().text);
        this->clear_logs_item->AddOnKey(std::bind(&OwnSettingsLayout::clear_logs_DefaultKey, this));

        this->settings_menu->AddItem(this->view_logs_item);
        this->settings_menu->AddItem(this->clear_logs_item);

        auto custom_lang_item_name = cfg::Strings.GetString(441) + ": ";
        if(g_Settings.lang == Language::Auto) {
            custom_lang_item_name += cfg::Strings.GetString(521);
        }
        else {
            custom_lang_item_name += LanguageNameList[static_cast<u32>(g_Settings.lang)];
        }
        this->custom_lang_item = pu::ui::elm::MenuItem::New(custom_lang_item_name);
        this->custom_lang_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->custom_lang_item->SetColor(g_Settings.GetColorScheme().text);
        this->custom_lang_item->AddOnKey(std::bind(&OwnSettingsLayout::custom_lang_DefaultKey, this));

        const auto use_12h_time_name = cfg::Strings.GetString(452) + ": " + (g_Settings.json_settings.general.value().use_12h_time.value() ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->use_12h_time_item = pu::ui::elm::MenuItem::New(use_12h_time_name);
        this->use_12h_time_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->use_12h_time_item->SetColor(g_Settings.GetColorScheme().text);
        this->use_12h_time_item->AddOnKey(std::bind(&OwnSettingsLayout::use_12h_time_DefaultKey, this));

        const auto ignore_hidden_files_name = cfg::Strings.GetString(474) + ": " + (g_Settings.json_settings.fs.value().ignore_hidden_files.value() ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->ignore_hidden_files_item = pu::ui::elm::MenuItem::New(ignore_hidden_files_name);
        this->ignore_hidden_files_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->ignore_hidden_files_item->SetColor(g_Settings.GetColorScheme().text);
        this->ignore_hidden_files_item->AddOnKey(std::bind(&OwnSettingsLayout::ignore_hidden_files_DefaultKey, this));
        
        this->settings_menu->AddItem(this->custom_lang_item);
        this->settings_menu->AddItem(this->use_12h_time_item);
        this->settings_menu->AddItem(this->ignore_hidden_files_item);

        const auto compute_directory_sizes_name = cfg::Strings.GetString(510) + ": " + (g_Settings.json_settings.fs.value().compute_directory_sizes.value() ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->compute_directory_sizes_item = pu::ui::elm::MenuItem::New(compute_directory_sizes_name);
        this->compute_directory_sizes_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->compute_directory_sizes_item->SetColor(g_Settings.GetColorScheme().text);
        this->compute_directory_sizes_item->AddOnKey(std::bind(&OwnSettingsLayout::compute_directory_sizes_DefaultKey, this));

        this->settings_menu->AddItem(this->compute_directory_sizes_item);

        const auto ignore_required_fw_version_item_name = cfg::Strings.GetString(444) + ": " + (g_Settings.json_settings.installs.value().ignore_required_fw_version.value() ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->ignore_required_fw_version_item = pu::ui::elm::MenuItem::New(ignore_required_fw_version_item_name);
        this->ignore_required_fw_version_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->ignore_required_fw_version_item->SetColor(g_Settings.GetColorScheme().text);
        this->ignore_required_fw_version_item->AddOnKey(std::bind(&OwnSettingsLayout::ignore_required_fw_version_DefaultKey, this));

        const auto show_deletion_prompt_after_install_name = cfg::Strings.GetString(475) + ": " + (g_Settings.json_settings.installs.value().show_deletion_prompt_after_install.value() ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
        this->show_deletion_prompt_after_install_item = pu::ui::elm::MenuItem::New(show_deletion_prompt_after_install_name);
        this->show_deletion_prompt_after_install_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->show_deletion_prompt_after_install_item->SetColor(g_Settings.GetColorScheme().text);
        this->show_deletion_prompt_after_install_item->AddOnKey(std::bind(&OwnSettingsLayout::show_deletion_prompt_after_install_DefaultKey, this));

        this->settings_menu->AddItem(this->ignore_required_fw_version_item);
        this->settings_menu->AddItem(this->show_deletion_prompt_after_install_item);

        std::string speed_fmt;
        GLEAF_ASSERT_TRUE(FormatMenuStickMoveSpeed(static_cast<cfg::MenuStickMoveSpeed>(g_Settings.json_settings.ui.value().menu_stick_move_speed.value()), speed_fmt));
        const auto menu_stick_move_speed_name = cfg::Strings.GetString(522) + ": " + speed_fmt;
        this->menu_stick_move_speed_item = pu::ui::elm::MenuItem::New(menu_stick_move_speed_name);
        this->menu_stick_move_speed_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        this->menu_stick_move_speed_item->SetColor(g_Settings.GetColorScheme().text);
        this->menu_stick_move_speed_item->AddOnKey(std::bind(&OwnSettingsLayout::menu_stick_move_speed_DefaultKey, this));

        this->settings_menu->AddItem(this->menu_stick_move_speed_item);

        std::string usb_speed_fmt;
        UsbDeviceSpeed usb_speed = UsbDeviceSpeed_None;
        if(hosversionAtLeast(8,0,0)) {
            usbDsGetSpeed(&usb_speed);
            GLEAF_ASSERT_TRUE(FormatUsbSpeed(usb_speed, usb_speed_fmt));
        }
        auto usb_speed_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(536) + " " + usb_speed_fmt);
        usb_speed_item->SetIcon(GetCommonIcon(CommonIconKind::Settings));
        usb_speed_item->SetColor(g_Settings.GetColorScheme().text);

        this->settings_menu->AddItem(usb_speed_item);

        if(!reset_selected_idx) {
            this->settings_menu->SetSelectedIndex(old_idx);
        }
    }

}
