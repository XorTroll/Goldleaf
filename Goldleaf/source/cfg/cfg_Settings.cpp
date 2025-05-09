
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

#include <cfg/cfg_Settings.hpp>
#include <fs/fs_FileSystem.hpp>
#include <ui/ui_Utils.hpp>

namespace cfg {

    namespace {

        inline std::string ColorToHex(const pu::ui::Color clr) {
            char str[0x20] = {};
            snprintf(str, sizeof(str), "#%02X%02X%02X%02X", clr.r, clr.g, clr.b, clr.a);
            return str;
        }

        Language g_DefaultLanguage = Language::English;
        bool g_DefaultLanguageLoaded = false;

        void EnsureDefaultLanguage() {
            if(!g_DefaultLanguageLoaded) {
                u64 tmp_lang_code = 0;
                auto sys_lang = SetLanguage_ENUS;
                GLEAF_RC_ASSERT(setGetSystemLanguage(&tmp_lang_code));
                GLEAF_RC_ASSERT(setMakeLanguage(tmp_lang_code, &sys_lang));
                g_DefaultLanguage = GetLanguageBySystemLanguage(sys_lang);
                g_DefaultLanguageLoaded = true;
            }

            GLEAF_ASSERT_TRUE(g_DefaultLanguageLoaded);
        }

        inline void ParseColorSchemeColor(const JSON &scheme_json, const char *name, pu::ui::Color &out_clr) {
            const auto raw_clr = scheme_json.value(name, "");
            if(!raw_clr.empty()) {
                out_clr = pu::ui::Color::FromHex(raw_clr);
            }
        }

        // Light/dark blue schemes by default

        constexpr ColorScheme DefaultLightScheme = {
            .bg = pu::ui::Color(0xAA, 0xFF, 0xFF, 0xFF),
            .menu_base = pu::ui::Color(0xCA, 0xFF, 0xFF, 0xFF),
            .menu_base_focus = pu::ui::Color(0xEA, 0xFB, 0xC2, 0xFF),
            .text = pu::ui::Color(0x00, 0x00, 0x00, 0xFF),
            .version_text = pu::ui::Color(0xFF, 0xC0, 0x00, 0xFF),
            .scroll_bar = pu::ui::Color(0xFF, 0xDA, 0x6C, 0xFF),
            .progress_bar = pu::ui::Color(0xFF, 0xC0, 0x00, 0xFF),
            .progress_bar_bg = pu::ui::Color(0xFF, 0xCF, 0x95, 0xFF),
            .dialog_title = pu::ui::Color(0x00, 0x00, 0x00, 0xFF),
            .dialog_opt = pu::ui::Color(0x2C, 0x2C, 0x2C, 0xFF),
            .dialog = pu::ui::Color(0xCA, 0xFF, 0xFF, 0xFF),
            .dialog_over = pu::ui::Color(0xEA, 0xFB, 0xC2, 0xFF)
        };
    
        constexpr ColorScheme DefaultDarkScheme = {
            .bg = pu::ui::Color(0x00, 0x2B, 0x87, 0xFF),
            .menu_base = pu::ui::Color(0x00, 0x31, 0x9A, 0xFF),
            .menu_base_focus = pu::ui::Color(0x8B, 0x66, 0x00, 0xFF),
            .text = pu::ui::Color(0xFF, 0xFF, 0xFF, 0xFF),
            .version_text = pu::ui::Color(0xB1, 0x82, 0x00, 0xFF),
            .scroll_bar = pu::ui::Color(0xD9, 0x91, 0x00, 0xFF),
            .progress_bar = pu::ui::Color(0xB1, 0x82, 0x00, 0xFF),
            .progress_bar_bg = pu::ui::Color(0xFF, 0xC0, 0x00, 0xFF),
            .dialog_title = pu::ui::Color(0xFF, 0xFF, 0xFF, 0xFF),
            .dialog_opt = pu::ui::Color(0xF0, 0xF0, 0xF0, 0xFF),
            .dialog = pu::ui::Color(0x00, 0x31, 0x9A, 0xFF),
            .dialog_over = pu::ui::Color(0x8B, 0x66, 0x00, 0xFF)
        };

        void ParseColorScheme(const JSON &json, ColorScheme &out_scheme) {
            ParseColorSchemeColor(json, "bg", out_scheme.bg);
            ParseColorSchemeColor(json, "menu_base", out_scheme.menu_base);
            ParseColorSchemeColor(json, "menu_base_focus", out_scheme.menu_base_focus);
            ParseColorSchemeColor(json, "text", out_scheme.text);
            ParseColorSchemeColor(json, "version_text", out_scheme.version_text);
            ParseColorSchemeColor(json, "scroll_bar", out_scheme.scroll_bar);
            ParseColorSchemeColor(json, "progress_bar", out_scheme.progress_bar);
            ParseColorSchemeColor(json, "progress_bar_bg", out_scheme.progress_bar_bg);
            ParseColorSchemeColor(json, "dialog_title", out_scheme.dialog_title);
            ParseColorSchemeColor(json, "dialog_opt", out_scheme.dialog_opt);
            ParseColorSchemeColor(json, "dialog", out_scheme.dialog);
            ParseColorSchemeColor(json, "dialog_over", out_scheme.dialog_over);
        }

        JSON GenerateColorScheme(const ColorScheme &scheme) {
            auto json = JSON::object();
            json["bg"] = ColorToHex(scheme.bg);
            json["menu_base"] = ColorToHex(scheme.menu_base);
            json["menu_base_focus"] = ColorToHex(scheme.menu_base_focus);
            json["text"] = ColorToHex(scheme.text);
            json["scroll_bar"] = ColorToHex(scheme.scroll_bar);
            json["progress_bar"] = ColorToHex(scheme.progress_bar);
            json["progress_bar_bg"] = ColorToHex(scheme.progress_bar_bg);
            json["dialog_title"] = ColorToHex(scheme.dialog_title);
            json["dialog_opt"] = ColorToHex(scheme.dialog_opt);
            json["dialog"] = ColorToHex(scheme.dialog);
            json["dialog_over"] = ColorToHex(scheme.dialog_over);
            return json;
        }

    }

    void Settings::Save() {
        auto json = JSON::object();

        json["general"]["language"] = GetLanguageCode(this->lang);
        json["general"]["external_romfs_path"] = this->external_romfs_path;
        json["general"]["use_12h_time"] = this->use_12h_time;
        json["general"]["ignore_hidden_files"] = this->ignore_hidden_files;

        json["ui"]["light_color_scheme"] = GenerateColorScheme(this->light_color_scheme);
        json["ui"]["dark_color_scheme"] = GenerateColorScheme(this->dark_color_scheme);
        json["ui"]["menu_item_size"] = this->menu_item_size;

        json["installs"]["ignore_required_fw_version"] = this->ignore_required_fw_ver;
        json["installs"]["show_deletion_prompt_after_install"] = this->show_deletion_prompt_after_install;
        json["installs"]["copy_buffer_max_size"] = this->copy_buffer_max_size;

        json["export"]["decrypt_buffer_max_size"] = this->decrypt_buffer_max_size;

        for(u32 i = 0; i < this->bookmarks.size(); i++) {
            const auto &bmk = this->bookmarks[i];
            json["web"]["bookmarks"][i]["name"] = bmk.name;
            json["web"]["bookmarks"][i]["url"] = bmk.url;
        }

        auto sd_exp = fs::GetSdCardExplorer();
        sd_exp->DeleteFile(GLEAF_PATH_SETTINGS_FILE);
        sd_exp->WriteJSON(GLEAF_PATH_SETTINGS_FILE, json);
    }

    std::string Settings::PathForResource(const std::string &res_path) {
        auto romfs_exp = fs::GetRomFsExplorer();
        
        if(!this->external_romfs_path.empty()) {
            const auto &ext_path = this->external_romfs_path + "/" + res_path;
            auto sd_exp = fs::GetSdCardExplorer();
            if(sd_exp->IsFile(ext_path)) {
                return ext_path;
            }
        }
        return romfs_exp->MakeAbsolute(res_path);
    }

    JSON Settings::ReadJSONResource(const std::string &res_path) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto romfs_exp = fs::GetRomFsExplorer();

        if(!this->external_romfs_path.empty()) {
            const auto &ext_path = this->external_romfs_path + "/" + res_path;
            if(sd_exp->IsFile(ext_path)) {
                return sd_exp->ReadJSON(ext_path);
            }
        }
        return romfs_exp->ReadJSON(romfs_exp->MakeAbsolute(res_path));
    }

    Settings ProcessSettings() {
        Settings settings = {
            .lang = Language::Auto,
            .external_romfs_path = "",
            .use_12h_time = false,
            .ignore_hidden_files = false,

            .light_color_scheme = DefaultLightScheme,
            .dark_color_scheme = DefaultDarkScheme,
            .menu_item_size = 100,

            .compute_directory_sizes = false,

            .ignore_required_fw_ver = true,
            .copy_buffer_max_size = 16_MB,

            .decrypt_buffer_max_size = 16_MB,

            .bookmarks = {}
        };

        ColorSetId sys_color_set_id;
        GLEAF_RC_ASSERT(setsysGetColorSetId(&sys_color_set_id));
        settings.system_is_light = sys_color_set_id == ColorSetId_Light;

        auto sd_exp = fs::GetSdCardExplorer();
        const auto settings_json = sd_exp->ReadJSON(GLEAF_PATH_SETTINGS_FILE);
        if(settings_json.count("general")) {
            const auto &lang = settings_json["general"].value("language", "");
            if(!lang.empty()) {
                settings.lang = GetLanguageByCode(lang);
            }

            const auto &ext_romfs_path = settings_json["general"].value("external_romfs_path", "");
            if(!ext_romfs_path.empty()) {
                if(ext_romfs_path.substr(0, 6) == "sdmc:/") {
                    settings.external_romfs_path = ext_romfs_path;
                }
                else {
                    settings.external_romfs_path = "sdmc:";
                    if(ext_romfs_path[0] != '/') {
                        settings.external_romfs_path += "/";
                    }
                    settings.external_romfs_path += ext_romfs_path;
                }
            }

            settings.use_12h_time = settings_json["general"].value("use_12h_time", settings.use_12h_time);
            settings.ignore_hidden_files = settings_json["general"].value("ignore_hidden_files", settings.ignore_hidden_files);
        }

        if(settings_json.count("ui")) {
            if(settings_json["ui"].count("light_color_scheme")) {
                ParseColorScheme(settings_json["ui"]["light_color_scheme"], settings.light_color_scheme);
            }
            if(settings_json["ui"].count("dark_color_scheme")) {
                ParseColorScheme(settings_json["ui"]["dark_color_scheme"], settings.dark_color_scheme);
            }

            settings.menu_item_size = settings_json["ui"].value("menu_item_size", settings.menu_item_size);
        }

        if(settings_json.count("fs")) {
            settings.compute_directory_sizes = settings_json["fs"].value("compute_directory_sizes", settings.compute_directory_sizes);
        }

        if(settings_json.count("installs")) {
            settings.ignore_required_fw_ver = settings_json["installs"].value("ignore_required_fw_version", settings.ignore_required_fw_ver);
            settings.show_deletion_prompt_after_install = settings_json["installs"].value("show_deletion_prompt_after_install", settings.show_deletion_prompt_after_install);
            settings.copy_buffer_max_size = settings_json["installs"].value("copy_buffer_max_size", settings.copy_buffer_max_size);
        }

        if(settings_json.count("export")) {
            settings.decrypt_buffer_max_size = settings_json["installs"].value("decrypt_buffer_max_size", settings.decrypt_buffer_max_size);
        }

        if(settings_json.count("web")) {
            if(settings_json["web"].count("bookmarks")) {
                for(u32 i = 0; i < settings_json["web"]["bookmarks"].size(); i++) {
                    const WebBookmark bmk = {
                        .name = settings_json["web"]["bookmarks"][i].value("name", ""),
                        .url = settings_json["web"]["bookmarks"][i].value("url", "")
                    };
                    if(!bmk.url.empty() && !bmk.name.empty()) {
                        settings.bookmarks.push_back(bmk);
                    }
                }
            }
        }

        return settings;
    }

    Language Settings::GetLanguage() {
        if(this->lang == Language::Auto) {
            EnsureDefaultLanguage();
            return g_DefaultLanguage;
        }
        else {
            return this->lang;
        }
    }

}
