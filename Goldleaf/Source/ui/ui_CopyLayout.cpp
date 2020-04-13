
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

#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    CopyLayout::CopyLayout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, cfg::strings::Main.GetString(151));
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(global_settings.custom_scheme.Text);
        this->copyBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->copyBar);
        this->Add(this->infoText);
        this->Add(this->copyBar);
    }

    void CopyLayout::StartCopy(String Path, String NewPath, bool Directory, fs::Explorer *Exp)
    {
        if(Directory)
        {
            fs::CopyDirectoryProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                global_app->CallForRender();
            });
            global_app->ShowNotification(cfg::strings::Main.GetString(141));
        }
        else
        {
            if(Exp->IsFile(NewPath))
            {
                int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(153), cfg::strings::Main.GetString(143), { cfg::strings::Main.GetString(239), cfg::strings::Main.GetString(18) }, true);
                if(sopt < 0) return;
            }
            fs::DeleteFile(NewPath);
            fs::CopyFileProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                global_app->CallForRender();
            });
            global_app->ShowNotification(cfg::strings::Main.GetString(240));
        }
    }
}