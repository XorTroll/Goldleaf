
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
            void ScrollUp();
            void ScrollDown();

            inline std::vector<String> ReadLines(u32 line_offset, u32 count) {
                if(this->read_hex) {
                    return this->file_exp->ReadFileFormatHex(this->path, line_offset, count);
                }
                else {
                    return this->file_exp->ReadFileLines(this->path, line_offset, count);
                }
            }

            inline void UpdateLines() {
                String new_cnt;
                for(auto &line: this->read_lines) {
                    new_cnt += line + "\n";
                }
                this->cnt_text->SetText(new_cnt);
            }
    };

}