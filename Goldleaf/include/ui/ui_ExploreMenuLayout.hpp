
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

    class ExploreMenuLayout : public pu::ui::Layout {
        private:
            struct PcSpecialPath {
                std::string name;
                std::string path;
            };

            pu::ui::elm::Menu::Ref mounts_menu;
            pu::ui::elm::MenuItem::Ref sd_card_menu_item;
            std::vector<pu::ui::elm::MenuItem::Ref> usb_drive_menu_items;
            pu::ui::elm::MenuItem::Ref remote_pc_explore_menu_item;
            std::unordered_map<fs::Explorer*, pu::ui::elm::MenuItem::Ref> mounted_explorer_items;
            pu::ui::elm::MenuItem::Ref nand_user_menu_item;
            pu::ui::elm::MenuItem::Ref nand_system_menu_item;
            pu::ui::elm::MenuItem::Ref nand_prodinfof_menu_item;
            pu::ui::elm::MenuItem::Ref nand_safe_menu_item;

            void ReloadMenu();

            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos);
            void OnSdCardSelected();
            void OnUsbDriveSelected(const UsbHsFsDevice drive);
            void OnUsbDriveSelectedX(const UsbHsFsDevice drive);
            void OnRemotePcExploreSelected();
            void OnNandPartitionSelected(const fs::Partition partition);
            void OnMountedExplorerSelected(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon);
            void OnMountedExplorerSelectedX(fs::Explorer *exp);

        public:
            ExploreMenuLayout();
            PU_SMART_CTOR(ExploreMenuLayout)

            void Reload();

            void AddMountedExplorer(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon);

            inline void AddMountedExplorerWithCommonIcon(fs::Explorer *exp, const std::string &name, const CommonIconKind kind) {
                this->AddMountedExplorer(exp, name, GetCommonIcon(kind));
            }
    };

}
