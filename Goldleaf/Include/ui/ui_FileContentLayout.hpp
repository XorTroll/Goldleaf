
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

    class FileContentLayout : public pu::ui::Layout {
        private:
            u32 line_offset;
            std::vector<String> read_lines;
            bool read_hex;
            String path;
            pu::ui::elm::TextBlock::Ref cnt_text;
            fs::Explorer *file_exp;
        public:
            FileContentLayout();
            PU_SMART_CTOR(FileContentLayout)

            void LoadFile(String path, fs::Explorer *exp, bool read_hex);
            void Update(bool insert_new_top);
            void ScrollUp();
            void ScrollDown();
    };

}