
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

    class MemoryLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::TextBlock::Ref sd_text;
            pu::ui::elm::ProgressBar::Ref sd_space_bar;
            pu::ui::elm::TextBlock::Ref sd_free_space_text;
            pu::ui::elm::TextBlock::Ref nand_text;
            pu::ui::elm::ProgressBar::Ref nand_space_bar;
            pu::ui::elm::TextBlock::Ref nand_free_text;
            pu::ui::elm::TextBlock::Ref nand_safe_text;
            pu::ui::elm::ProgressBar::Ref nand_safe_space_bar;
            pu::ui::elm::TextBlock::Ref nand_system_text;
            pu::ui::elm::ProgressBar::Ref nand_system_space_bar;
            pu::ui::elm::TextBlock::Ref nand_user_text;
            pu::ui::elm::ProgressBar::Ref nand_user_space_bar;
        public:
            MemoryLayout();
            PU_SMART_CTOR(MemoryLayout)

            void UpdateElements();
    };

}