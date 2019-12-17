
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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
#include <pu/Plutonium>

namespace ui
{
    class MainMenuLayout : public pu::ui::Layout
    {
        public:
            MainMenuLayout();
            PU_SMART_CTOR(MainMenuLayout)

            void exploreMenuItem_Click();
            void titleMenuItem_Click();
            void webMenuItem_Click();
            void accountMenuItem_Click();
            void amiiboMenuItem_Click();
            void emuiiboMenuItem_Click();
            void settingsMenuItem_Click();
            void updateMenuItem_Click();
            void aboutMenuItem_Click();
        private:
            pu::ui::elm::Menu::Ref optionMenu;
            pu::ui::elm::MenuItem::Ref exploreMenuItem;
            pu::ui::elm::MenuItem::Ref titleMenuItem;
            pu::ui::elm::MenuItem::Ref webMenuItem;
            pu::ui::elm::MenuItem::Ref accountMenuItem;
            pu::ui::elm::MenuItem::Ref amiiboMenuItem;
            pu::ui::elm::MenuItem::Ref emuiiboMenuItem;
            pu::ui::elm::MenuItem::Ref settingsMenuItem;
            pu::ui::elm::MenuItem::Ref updateMenuItem;
            pu::ui::elm::MenuItem::Ref aboutMenuItem;
    };
}