
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
    class UnusedTicketsLayout : public pu::ui::Layout
    {
        public:
            UnusedTicketsLayout();
            PU_SMART_CTOR(UnusedTicketsLayout)
            
            void UpdateElements(bool Cooldown);
            void tickets_Click(hos::Ticket ticket);
        private:
            std::vector<hos::Ticket> tickets;
            pu::ui::elm::TextBlock::Ref notTicketsText;
            pu::ui::elm::Menu::Ref ticketsMenu;
    };
}