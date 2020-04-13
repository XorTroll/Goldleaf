
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

#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    FileContentLayout::FileContentLayout()
    {
        this->cntText = pu::ui::elm::TextBlock::New(40, 180, "");
        this->cntText->SetColor(global_settings.custom_scheme.Text);
        this->cntText->SetFont("FileContentFont");
        this->Add(this->cntText);
        this->loffset = 0;
    }

    void FileContentLayout::LoadFile(String PPath, String Path, fs::Explorer *Exp, bool Hex)
    {
        this->pth = Path;
        this->mode = Hex;
        this->gexp = Exp;
        this->loffset = 0;
        this->Update();
    }

    void FileContentLayout::Update()
    {
        std::vector<String> lines;
        if(this->mode) lines = this->gexp->ReadFileFormatHex(this->pth, this->loffset, 19);
        else lines = this->gexp->ReadFileLines(this->pth, this->loffset, 19);
        if(lines.empty())
        {
            this->loffset--;
            return;
        }
        this->rlines = lines.size();
        String alines;
        if(this->rlines > 0) for(u32 i = 0; i < this->rlines; i++)
        {
            if(i > 0) alines += "\n";
            alines += lines[i];
        }
        this->cntText->SetText(alines);
        lines.clear();
    }

    void FileContentLayout::ScrollUp()
    {
        if(this->loffset > 0)
        {
            this->loffset--;
            this->Update();
        }
    }

    void FileContentLayout::ScrollDown()
    {
        this->loffset++;
        this->Update();
    }
}