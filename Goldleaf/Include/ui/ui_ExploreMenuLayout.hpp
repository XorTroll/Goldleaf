
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
    class ExploreMenuLayout : public pu::ui::Layout
    {
        public:
            ExploreMenuLayout();
            PU_SMART_CTOR(ExploreMenuLayout)

            void mountsMenu_SelectionChanged();
            void sdCard_Click();
            void pcDrive_Click();
            void usbDrive_Click();
            void nandProdInfoF_Click();
            void nandSafe_Click();
            void nandUser_Click();
            void nandSystem_Click();
            void otherMount_Click();
            void specialMount_Click_X();
            void otherMount_Click_X();
        private:
            pu::ui::elm::Menu::Ref mountsMenu;
            pu::ui::elm::MenuItem::Ref sdCardMenuItem;
            pu::ui::elm::MenuItem::Ref pcDriveMenuItem;
            pu::ui::elm::MenuItem::Ref usbDriveMenuItem;
            pu::ui::elm::MenuItem::Ref nandProfInfoFMenuItem;
            pu::ui::elm::MenuItem::Ref nandSafeMenuItem;
            pu::ui::elm::MenuItem::Ref nandUserMenuItem;
            pu::ui::elm::MenuItem::Ref nandSystemMenuItem;
            std::vector<pu::ui::elm::MenuItem::Ref> mounts;
            std::vector<fs::Explorer*> expls;
    };
}