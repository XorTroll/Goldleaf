
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
extern cfg::Settings global_settings;
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
        this->infoText->SetText(cfg::strings::Main.GetString(305));
        global_app->CallForRender();
        std::string js = net::RetrieveContent("https://api.github.com/repos/xortroll/goldleaf/releases", "application/json");
        JSON j = JSON::parse(js);
        std::string latestid = j[0]["tag_name"].get<std::string>();
        this->infoText->SetText(cfg::strings::Main.GetString(306));
        global_app->CallForRender();
        Version latestv = Version::FromString(latestid);
        Version currentv = Version::MakeVersion(GOLDLEAF_MAJOR, GOLDLEAF_MINOR, GOLDLEAF_MICRO); // Defined in Makefile
        if(latestv.IsEqual(currentv)) global_app->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(307), { cfg::strings::Main.GetString(234) }, true);
        else if(latestv.IsLower(currentv))
        {
            int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(308), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
            if(sopt == 0)
            {
                std::string newnro = "https://github.com/XorTroll/Goldleaf/releases/download/" + latestid + "/Goldleaf.nro";
                fs::CreateDirectory("sdmc:/switch/Goldleaf");
                fs::DeleteFile("sdmc:/" + consts::Root + "/update_tmp.nro");
                this->infoText->SetText(cfg::strings::Main.GetString(309));
                global_app->CallForRender();
                this->downloadBar->SetVisible(true);
                net::RetrieveToFile(newnro, "sdmc:/" + consts::Root + "/update_tmp.nro", [&](double Done, double Total)
                {
                    this->downloadBar->SetMaxValue(Total);
                    this->downloadBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                if(fs::IsFile("sdmc:/" + consts::Root + "/update_tmp.nro")) gupdated = true;
                this->downloadBar->SetVisible(false);
                global_app->CallForRender();
                global_app->ShowNotification(cfg::strings::Main.GetString(314) + " " + cfg::strings::Main.GetString(315));
            }
        }
        else if(latestv.IsHigher(currentv)) global_app->CreateShowDialog(cfg::strings::Main.GetString(284), cfg::strings::Main.GetString(316), { cfg::strings::Main.GetString(234) }, true);
        global_app->ReturnToMainMenu();
    }
}