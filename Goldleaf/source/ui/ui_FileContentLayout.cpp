
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2025 XorTroll

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

    void FileContentLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
        else if((keys_down & HidNpadButton_Down) || (keys_down & HidNpadButton_StickLDown) || (keys_held & HidNpadButton_StickRDown)) {
            this->ScrollDown();
        }
        else if((keys_down & HidNpadButton_Up) || (keys_down & HidNpadButton_StickLUp) || (keys_held & HidNpadButton_StickRUp)) {
            this->ScrollUp();
        }
    }

    FileContentLayout::FileContentLayout() {
        this->cnt_text = pu::ui::elm::TextBlock::New(15, 290, "0");
        this->cnt_text->SetColor(g_Settings.GetColorScheme().text);
        this->cnt_text->SetFont("FileContentFont");
        this->Add(this->cnt_text);
        this->line_offset = 0;
        this->available_line_count = (pu::ui::render::ScreenHeight - this->cnt_text->GetY()) / this->cnt_text->GetHeight();

        this->SetOnInput(std::bind(&FileContentLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void FileContentLayout::LoadFile(const std::string &path, const std::string &pres_path, fs::Explorer *exp, const bool read_hex) {
        this->path = path;
        this->read_hex = read_hex;
        this->file_exp = exp;
        this->line_offset = 0;

        if(read_hex) {
            g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(507), CommonIconKind::BinaryFile, cfg::Strings.GetString(468) + ": " + pres_path);
        }
        else {
            g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(508), CommonIconKind::TextFile, cfg::Strings.GetString(467) + ": " + pres_path);
        }
        
        this->read_lines = this->ReadLines(this->line_offset, this->available_line_count);
        this->UpdateLines();
    }

    void FileContentLayout::ScrollUp() {
        if(this->line_offset > 0) {
            if(this->read_lines.size() >= this->available_line_count) {
                this->read_lines.pop_back();
            }
            this->line_offset--;

            const auto new_line_l = this->ReadLines(this->line_offset, 1);
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

            const auto new_line_l = this->ReadLines(this->line_offset + this->read_lines.size(), 1);
            if(!new_line_l.empty()) {
                this->read_lines.push_back(new_line_l.front());
            }
            this->UpdateLines();
        }
    }

}
