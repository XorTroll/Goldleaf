
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

    class MainMenuLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::Menu::Ref options_menu;
            pu::ui::elm::MenuItem::Ref explore_menu_item;
            pu::ui::elm::MenuItem::Ref cnt_manager_menu_item;
            pu::ui::elm::MenuItem::Ref web_browser_menu_item;
            pu::ui::elm::MenuItem::Ref account_menu_item;
            pu::ui::elm::MenuItem::Ref amiibo_menu_item;
            pu::ui::elm::MenuItem::Ref settings_menu_item;
            pu::ui::elm::MenuItem::Ref update_menu_item;
            pu::ui::elm::MenuItem::Ref about_menu_item;
        public:
            MainMenuLayout();
            PU_SMART_CTOR(MainMenuLayout)

            void exploreMenu_DefaultKey();
            void contentManager_DefaultKey();
            void webBrowser_DefaultKey();
            void account_DefaultKey();
            void amiibo_DefaultKey();
            void settings_DefaultKey();
            void update_DefaultKey();
            void about_DefaultKey();
    };

}