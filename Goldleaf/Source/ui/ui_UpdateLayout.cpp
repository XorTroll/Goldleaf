
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

#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern set::Settings global_settings;
extern bool gupdated;

namespace ui
{
    UpdateLayout::UpdateLayout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, "(...)");
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(global_settings.custom_scheme.Text);
        this->downloadBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->downloadBar);
        this->Add(this->infoText);
        this->Add(this->downloadBar);
    }

    void UpdateLayout::StartUpdateSearch()
    {
        if(gupdated) return;
        this->downloadBar->SetVisible(false);
        this->infoText->SetText(set::GetDictionaryEntry(305));
        global_app->CallForRender();
        std::string js = net::RetrieveContent("https://api.github.com/repos/xortroll/goldleaf/releases", "application/json");
        JSON j = JSON::parse(js);
        std::string latestid = j[0]["tag_name"].get<std::string>();
        this->infoText->SetText(set::GetDictionaryEntry(306));
        global_app->CallForRender();
        Version latestv = Version::FromString(latestid);
        Version currentv = Version::MakeVersion(GOLDLEAF_MAJOR, GOLDLEAF_MINOR, GOLDLEAF_MICRO); // Defined in Makefile
        if(latestv.IsEqual(currentv)) global_app->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(307), { set::GetDictionaryEntry(234) }, true);
        else if(latestv.IsLower(currentv))
        {
            int sopt = global_app->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(308), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt == 0)
            {
                std::string newnro = "https://github.com/XorTroll/Goldleaf/releases/download/" + latestid + "/Goldleaf.nro";
                fs::CreateDirectory("sdmc:/switch/Goldleaf");
                fs::DeleteFile(consts::TempUpdatePath);
                this->infoText->SetText(set::GetDictionaryEntry(309));
                global_app->CallForRender();
                this->downloadBar->SetVisible(true);
                net::RetrieveToFile(newnro, consts::TempUpdatePath, [&](double Done, double Total)
                {
                    this->downloadBar->SetMaxValue(Total);
                    this->downloadBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                if(fs::IsFile(consts::TempUpdatePath)) gupdated = true;
                this->downloadBar->SetVisible(false);
                global_app->CallForRender();
                global_app->ShowNotification(set::GetDictionaryEntry(314) + " " + set::GetDictionaryEntry(315));
            }
        }
        else if(latestv.IsHigher(currentv)) global_app->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(316), { set::GetDictionaryEntry(234) }, true);
        global_app->UnloadMenuData();
        global_app->LoadLayout(global_app->GetMainMenuLayout());
    }
}