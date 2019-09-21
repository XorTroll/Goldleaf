
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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

#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    CopyLayout::CopyLayout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, set::GetDictionaryEntry(151));
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->copyBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->copyBar);
        this->Add(this->infoText);
        this->Add(this->copyBar);
    }

    void CopyLayout::StartCopy(pu::String Path, pu::String NewPath, bool Directory, fs::Explorer *Exp)
    {
        if(Directory)
        {
            fs::CopyDirectoryProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(141));
        }
        else
        {
            if(Exp->IsFile(NewPath))
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(153), set::GetDictionaryEntry(143), { set::GetDictionaryEntry(239), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
            }
            fs::DeleteFile(NewPath);
            fs::CopyFileProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(240));
        }
    }
}