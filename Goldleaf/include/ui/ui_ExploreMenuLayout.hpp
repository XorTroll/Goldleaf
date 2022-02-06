
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

#pragma once
#include <ui/ui_Includes.hpp>

namespace ui {

    class ExploreMenuLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::Menu::Ref mounts_menu;
            pu::ui::elm::MenuItem::Ref sd_card_menu_item;
            pu::ui::elm::MenuItem::Ref remote_pc_menu_item;
            pu::ui::elm::MenuItem::Ref usb_drive_menu_item;
            pu::ui::elm::MenuItem::Ref nand_prodinfof_menu_item;
            pu::ui::elm::MenuItem::Ref nand_safe_menu_item;
            pu::ui::elm::MenuItem::Ref nand_user_menu_item;
            pu::ui::elm::MenuItem::Ref nand_system_menu_item;
            std::vector<fs::Explorer*> mounted_explorers;
            std::vector<pu::ui::elm::MenuItem::Ref> mounted_explorer_items;

        public:
            ExploreMenuLayout();
            ~ExploreMenuLayout();
            PU_SMART_CTOR(ExploreMenuLayout)

            void UpdateMenu();
            void mountsMenu_SelectionChanged();
            void sdCard_DefaultKey();
            void pcDrive_DefaultKey();
            void usbDrive_DefaultKey();
            void nandProdInfoF_DefaultKey();
            void nandSafe_DefaultKey();
            void nandUser_DefaultKey();
            void nandSystem_DefaultKey();
            void otherMount_DefaultKey();
            void specialMount_X();
            void otherMount_X();
            void explorer_DefaultKey(fs::Explorer *exp, const std::string &name, const std::string &icon);
            void explorer_X(fs::Explorer *exp);
            void AddMountedExplorer(fs::Explorer *exp, const std::string &name, const std::string &icon);
            std::vector<fs::Explorer*> &GetMountedExplorers();
    };

}