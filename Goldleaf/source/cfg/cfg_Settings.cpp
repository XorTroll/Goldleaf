
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

        #define _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, name) \
            if(json_scheme.name.has_value()) { \
                out_scheme.name = pu::ui::Color::FromHex(json_scheme.name.value()); \
            } \
            else { \
                out_scheme.name = def_scheme.name; \
            } \

        void ParseColorScheme(const json::UiColorScheme &json_scheme, ColorScheme &out_scheme, const ColorScheme &def_scheme) {
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, bg);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, menu_base);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, menu_base_focus);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, text);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, version_text);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, scroll_bar);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, progress_bar);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, progress_bar_bg);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, dialog_title);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, dialog_opt);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, dialog);
            _CFG_PARSE_COLOR_SCHEME_COLOR(json_scheme, out_scheme, def_scheme, dialog_over);
        }

        #define _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, name) out_json_scheme.name = ColorToHex(scheme.name)

        void GenerateColorScheme(const ColorScheme &scheme, json::UiColorScheme &out_json_scheme, const ColorScheme &def_scheme) {
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, bg);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, menu_base);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, menu_base_focus);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, text);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, version_text);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, scroll_bar);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, progress_bar);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, progress_bar_bg);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, dialog_title);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, dialog_opt);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, dialog);
            _CFG_GEN_COLOR_SCHEME_COLOR(scheme, out_json_scheme, def_scheme, dialog_over);
        }

    }

    #define _CFG_ENSURE_SETTING(json_settings, setting, type) \
        if(!json_settings.setting.has_value()) { \
            json_settings.setting = json::type::MakeDefault(); \
        }

    void Settings::Load() {
        _CFG_ENSURE_SETTING(this->json_settings, general, GeneralSettings);
        _CFG_ENSURE_SETTING(this->json_settings, ui, UiSettings);
        _CFG_ENSURE_SETTING(this->json_settings, fs, FsSettings);
        _CFG_ENSURE_SETTING(this->json_settings, installs, InstallsSettings);
        _CFG_ENSURE_SETTING(this->json_settings, exports, ExportsSettings);
        _CFG_ENSURE_SETTING(this->json_settings, web, WebSettings);

        const auto err = glz::read_file_json<PartialJsonOptions{}>(this->json_settings, "sdmc:/" GLEAF_PATH_SETTINGS_FILE, std::string{});
        if(err) {
            GLEAF_WARN_FMT("Failed to read settings JSON: %d (%s)", (u32)err.ec, err.custom_error_message.data());
        }

        if(this->json_settings.general.value().language.has_value()) {
            this->lang = GetLanguageByCode(this->json_settings.general.value().language.value());
        }
        else {
            this->lang = Language::Auto;
        }

        ColorSetId sys_color_set_id;
        GLEAF_RC_ASSERT(setsysGetColorSetId(&sys_color_set_id));
        this->is_light_mode = sys_color_set_id == ColorSetId_Light;

        if(this->json_settings.ui.value().light_color_scheme.has_value()) {
            ParseColorScheme(this->json_settings.ui.value().light_color_scheme.value(), this->light_color_scheme, DefaultLightScheme);
        }
        else {
            this->light_color_scheme = DefaultLightScheme;
        }

        if(this->json_settings.ui.value().dark_color_scheme.has_value()) {
            ParseColorScheme(this->json_settings.ui.value().dark_color_scheme.value(), this->dark_color_scheme, DefaultDarkScheme);
        }
        else {
            this->dark_color_scheme = DefaultDarkScheme;
        }

        #define _CFG_CLAMP_FS_BUFFER_SIZE(mod, name) \
            if(this->json_settings.mod.value().name.value() > 12_MB) { \
                this->json_settings.mod.value().name.value() = 12_MB; \
            } \

        _CFG_CLAMP_FS_BUFFER_SIZE(installs, copy_buffer_max_size);
        _CFG_CLAMP_FS_BUFFER_SIZE(exports, decrypt_buffer_max_size);
    }

    void Settings::Save() {
        this->json_settings.general.value().language = GetLanguageCode(this->lang);

        this->json_settings.ui.value().light_color_scheme = json::UiColorScheme{};
        GenerateColorScheme(this->light_color_scheme, this->json_settings.ui.value().light_color_scheme.value(), DefaultLightScheme);

        this->json_settings.ui.value().dark_color_scheme = json::UiColorScheme{};
        GenerateColorScheme(this->dark_color_scheme, this->json_settings.ui.value().dark_color_scheme.value(), DefaultDarkScheme);

        GLEAF_ASSERT_TRUE(!glz::write_file_json<PartialJsonOptions{}>(this->json_settings, "sdmc:/" GLEAF_PATH_SETTINGS_FILE, std::string{}));
    }

    std::string Settings::PathForResource(const std::string &res_path) {
        auto romfs_exp = fs::GetRomFsExplorer();
        
        if(this->json_settings.general.value().external_romfs_path.has_value()) {
            const auto &ext_path = this->json_settings.general.value().external_romfs_path.value() + "/" + res_path;
            auto sd_exp = fs::GetSdCardExplorer();
            if(sd_exp->IsFile(ext_path)) {
                return ext_path;
            }
        }
        return romfs_exp->MakeAbsolute(res_path);
    }

    StringHolder Settings::ReadStrings(const Language lang) {
        StringHolder str_holder = {
            .language = lang,
            .strings = {}
        };

        auto romfs_exp = fs::GetRomFsExplorer();
        GLEAF_ASSERT_TRUE(!glz::read_file_json<PartialJsonOptions{}>(str_holder.strings, romfs_exp->MakeAbsolute("/Strings/" + GetLanguageCode(lang) + ".json"), std::string{}));

        return str_holder;
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
