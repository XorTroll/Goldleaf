
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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
    class SettingsLayout : public pu::ui::Layout
    {
        public:
            SettingsLayout();
            PU_SMART_CTOR(SettingsLayout)

            void ExportUpdateToDirectory(String Input, SetSysFirmwareVersion Fw);
            void ExportUpdateToNSP(String Input, SetSysFirmwareVersion Fw);
            void HandleUpdate(String Base, SetSysFirmwareVersion Fw);
            void optsConfig_Click();
            void optsFirmware_Click();
            void optsMemory_Click();
        private:
            pu::ui::elm::Menu::Ref optsMenu;
            pu::ui::elm::ProgressBar::Ref progressInfo;
    };
}