
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

#pragma once
#include <ui/ui_Includes.hpp>

namespace ui {

    class ContentManagerLayout : public pu::ui::Layout
    {
        private:
            pu::ui::elm::MenuItem::Ref sd_card_menu_item;
            pu::ui::elm::MenuItem::Ref nand_user_menu_item;
            pu::ui::elm::MenuItem::Ref nand_system_menu_item;
            pu::ui::elm::MenuItem::Ref gamecart_menu_item;
            pu::ui::elm::MenuItem::Ref unused_tickets_menu_item;
            pu::ui::elm::Menu::Ref types_menu;
        public:
            ContentManagerLayout();
            PU_SMART_CTOR(ContentManagerLayout)

            void sdCardMenuItem_Click();
            void nandUserMenuItem_Click();
            void nandSystemMenuItem_Click();
            void gameCartMenuItem_Click();
            void unusedTicketsMenuItem_Click();
    };

}