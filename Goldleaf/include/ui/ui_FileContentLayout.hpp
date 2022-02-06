
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

    class FileContentLayout : public pu::ui::Layout {
        private:
            u32 line_offset;
            std::vector<std::string> read_lines;
            bool read_hex;
            std::string path;
            pu::ui::elm::TextBlock::Ref cnt_text;
            fs::Explorer *file_exp;

        public:
            FileContentLayout();
            PU_SMART_CTOR(FileContentLayout)

            void LoadFile(const std::string &path, fs::Explorer *exp, const bool read_hex);
            void ScrollUp();
            void ScrollDown();

            inline std::vector<std::string> ReadLines(const u32 line_offset, const u32 count) {
                if(this->read_hex) {
                    return this->file_exp->ReadFileFormatHex(this->path, line_offset, count);
                }
                else {
                    return this->file_exp->ReadFileLines(this->path, line_offset, count);
                }
            }

            inline void UpdateLines() {
                std::string new_cnt;
                for(auto &line: this->read_lines) {
                    new_cnt += line + "\n";
                }
                this->cnt_text->SetText(new_cnt);
            }
    };

}