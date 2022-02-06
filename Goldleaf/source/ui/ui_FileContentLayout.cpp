
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

#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        constexpr u32 MaxLineCount = 19;

    }

    FileContentLayout::FileContentLayout() {
        this->cnt_text = pu::ui::elm::TextBlock::New(40, 180, "");
        this->cnt_text->SetColor(g_Settings.custom_scheme.text);
        this->cnt_text->SetFont("FileContentFont");
        this->Add(this->cnt_text);
        this->line_offset = 0;
    }

    void FileContentLayout::LoadFile(const std::string &path, fs::Explorer *exp, const bool read_hex) {
        this->path = path;
        this->read_hex = read_hex;
        this->file_exp = exp;
        this->line_offset = 0;

        this->read_lines = this->ReadLines(this->line_offset, MaxLineCount);
        this->UpdateLines();
    }

    void FileContentLayout::ScrollUp() {
        if(this->line_offset > 0) {
            if(this->read_lines.size() >= MaxLineCount) {
                this->read_lines.pop_back();
            }
            this->line_offset--;

            auto new_line_l = this->ReadLines(this->line_offset, 1);
            if(!new_line_l.empty()) {
                this->read_lines.insert(this->read_lines.begin(), new_line_l.front());
            }
            this->UpdateLines();
        }
    }

    void FileContentLayout::ScrollDown() {
        if(!this->read_lines.empty()) {
            this->read_lines.erase(this->read_lines.begin());
            this->line_offset++;

            auto new_line_l = this->ReadLines(this->line_offset + this->read_lines.size(), 1);
            if(!new_line_l.empty()) {
                this->read_lines.push_back(new_line_l.front());
            }
            this->UpdateLines();
        }
    }

}