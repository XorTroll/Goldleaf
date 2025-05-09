
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
#include <ui/ui_Includes.hpp>

namespace ui {

    class OwnSettingsLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::Menu::Ref settings_menu;

            pu::ui::elm::MenuItem::Ref view_logs_item;
            
            void view_logs_DefaultKey();

            // General
            pu::ui::elm::MenuItem::Ref custom_lang_item;
            pu::ui::elm::MenuItem::Ref use_12h_time_item;
            pu::ui::elm::MenuItem::Ref ignore_hidden_files_item;

            void custom_lang_DefaultKey();
            void use_12h_time_DefaultKey();
            void ignore_hidden_files_DefaultKey();

            // FS
            pu::ui::elm::MenuItem::Ref compute_directory_sizes_item;

            void compute_directory_sizes_DefaultKey();

            // Installs
            pu::ui::elm::MenuItem::Ref ignore_required_fw_version_item;
            pu::ui::elm::MenuItem::Ref show_deletion_prompt_after_install_item;

            void ignore_required_fw_version_DefaultKey();
            void show_deletion_prompt_after_install_DefaultKey();

            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos);
        public:
            OwnSettingsLayout();
            PU_SMART_CTOR(OwnSettingsLayout)

            void UpdateSettings(const bool reset_selected_idx);
    };

}
