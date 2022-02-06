
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

    class CopyLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::ProgressBar::Ref copy_total_p_bar;
            pu::ui::elm::TextBlock::Ref info_text;
            pu::ui::elm::ProgressBar::Ref copy_file_p_bar;

        public:
            CopyLayout();
            PU_SMART_CTOR(CopyLayout)

            void StartCopy(const std::string &path, const std::string &new_path);
    };

}