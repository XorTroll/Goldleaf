
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

#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        constexpr u32 ReadLineCount = 19;

    }

    FileContentLayout::FileContentLayout() {
        this->cnt_text = pu::ui::elm::TextBlock::New(40, 180, "");
        this->cnt_text->SetColor(g_Settings.custom_scheme.Text);
        this->cnt_text->SetFont("FileContentFont");
        this->Add(this->cnt_text);
        this->line_offset = 0;
    }

    void FileContentLayout::LoadFile(String path, fs::Explorer *exp, bool read_hex) {
        this->path = path;
        this->read_hex = read_hex;
        this->file_exp = exp;
        this->line_offset = 0;
        this->read_lines.clear();
        this->Update(false);
    }

    void FileContentLayout::Update(bool insert_new_top) {
        // TODO: fix this
        std::vector<String> new_lines;
        const auto read_line_count = ReadLineCount - this->read_lines.size();
        if(this->read_hex) {
            new_lines = this->file_exp->ReadFileFormatHex(this->path, this->line_offset + this->read_lines.size(), read_line_count);
        }
        else {
            new_lines = this->file_exp->ReadFileLines(this->path, this->line_offset + this->read_lines.size(), read_line_count);
        }
        if(insert_new_top) {
            this->read_lines.insert(this->read_lines.begin(), new_lines.begin(), new_lines.end());
        }
        else {
            this->read_lines.insert(this->read_lines.end(), new_lines.begin(), new_lines.end());
        }

        String new_cnt;
        for(auto &line: this->read_lines) {
            new_cnt += line + "\n";
        }
        this->cnt_text->SetText(new_cnt);
    }

    void FileContentLayout::ScrollUp() {
        if(this->line_offset > 0) {
            this->line_offset--;
            this->read_lines.pop_back();
            this->Update(true);
        }
    }

    void FileContentLayout::ScrollDown() {
        this->line_offset++;
        this->read_lines.erase(this->read_lines.begin());
        this->Update(false);
    }

}