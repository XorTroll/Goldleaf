
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

#pragma once
#include <cfg/cfg_Strings.hpp>

namespace cfg {

    namespace json {

        struct GeneralSettings {
            std::optional<std::string> language;
            std::optional<std::string> external_romfs_path;
            std::optional<bool> use_12h_time;

            static inline GeneralSettings MakeDefault() {
                return {
                    .language = std::nullopt,
                    .external_romfs_path = std::nullopt,
                    .use_12h_time = false
                };
            }
        };

        struct UiColorScheme {
            std::optional<std::string> bg;
            std::optional<std::string> menu_base;
            std::optional<std::string> menu_base_focus;
            std::optional<std::string> text;
            std::optional<std::string> version_text;
            std::optional<std::string> scroll_bar;
            std::optional<std::string> progress_bar;
            std::optional<std::string> progress_bar_bg;
            std::optional<std::string> dialog_title;
            std::optional<std::string> dialog_opt;
            std::optional<std::string> dialog;
            std::optional<std::string> dialog_over;
        };

        struct UiSettings {
            std::optional<UiColorScheme> light_color_scheme;
            std::optional<UiColorScheme> dark_color_scheme;
            std::optional<u32> menu_item_size;

            static inline UiSettings MakeDefault() {
                return {
                    .light_color_scheme = std::nullopt,
                    .dark_color_scheme = std::nullopt,
                    .menu_item_size = 100
                };
            }
        };

        struct FsSettings {
            std::optional<bool> compute_directory_sizes;
            std::optional<bool> ignore_hidden_files;

            static inline FsSettings MakeDefault() {
                return {
                    .compute_directory_sizes = false,
                    .ignore_hidden_files = false
                };
            }
        };

        struct InstallsSettings {
            std::optional<bool> ignore_required_fw_version;
            std::optional<bool> show_deletion_prompt_after_install;
            std::optional<size_t> copy_buffer_max_size;

            static inline InstallsSettings MakeDefault() {
                return {
                    .ignore_required_fw_version = true,
                    .show_deletion_prompt_after_install = true,
                    .copy_buffer_max_size = 8_MB
                };
            }
        };

        struct ExportsSettings {
            std::optional<size_t> decrypt_buffer_max_size;

            static inline ExportsSettings MakeDefault() {
                return {
                    .decrypt_buffer_max_size = 8_MB
                };
            }
        };

        struct WebBookmark {
            std::string name;
            std::string url;
        };

        struct WebSettings {
            std::optional<std::vector<WebBookmark>> bookmarks;

            static inline WebSettings MakeDefault() {
                return {
                    .bookmarks = std::vector<WebBookmark>{}
                };
            }
        };

        struct Settings {
            std::optional<GeneralSettings> general;
            std::optional<UiSettings> ui;
            std::optional<FsSettings> fs;
            std::optional<InstallsSettings> installs;
            std::optional<ExportsSettings> exports;
            std::optional<WebSettings> web;
        };

    }

    struct Settings {
        json::Settings json_settings;

        // Cached values
        Language lang;
        ColorScheme light_color_scheme;
        ColorScheme dark_color_scheme;
        bool is_light_mode;

        inline ColorScheme &GetColorScheme() {
            return this->is_light_mode ? this->light_color_scheme : this->dark_color_scheme;
        }

        inline void ApplyToMenu(pu::ui::elm::Menu::Ref menu) {
            menu->SetScrollbarColor(this->GetColorScheme().scroll_bar);
            menu->SetMoveWaitTimeMs(50);
            menu->SetItemAlphaIncrementSteps(5);
        }

        void Load();
        void Save();
        std::string PathForResource(const std::string &res_path);
        StringHolder ReadStrings(const Language lang);
        Language GetLanguage();
    };

}
