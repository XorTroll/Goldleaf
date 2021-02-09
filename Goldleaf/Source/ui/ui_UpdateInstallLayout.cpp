
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

#include <ui/ui_UpdateInstallLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <hos/hos_Payload.hpp>
#include <amssu/amssu_Service.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    UpdateInstallLayout::UpdateInstallLayout() : pu::ui::Layout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, cfg::strings::Main.GetString(151));
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(global_settings.custom_scheme.Text);
        this->processBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->processBar->SetVisible(false);
        global_settings.ApplyProgressBarColor(this->processBar);
        this->Add(this->infoText);
        this->Add(this->processBar);
    }

    void UpdateInstallLayout::InstallUpdate(String path, bool with_exfat)
    {
        this->infoText->SetText(cfg::strings::Main.GetString(426));
        global_app->CallForRender();
        this->processBar->SetVisible(true);
        hos::UnlockAutoSleep();
        auto rc = amssu::SetupUpdate(nullptr, UpdateWorkBufferSize, path.AsUTF8().c_str(), with_exfat);
        if(R_SUCCEEDED(rc))
        {
            AsyncResult async_rc;
            rc = amssu::RequestPrepareUpdate(&async_rc);
            if(R_SUCCEEDED(rc))
            {
                auto wait_ok = false;
                while(true)
                {
                    rc = asyncResultWait(&async_rc, 0);
                    if(R_SUCCEEDED(rc))
                    {
                        rc = asyncResultGet(&async_rc);
                        wait_ok = R_SUCCEEDED(rc);
                        break;
                    }
                    else if(rc == 0xEA01)
                    {
                        bool prepared = false;
                        amssu::HasPreparedUpdate(&prepared);
                        if(prepared)
                        {
                            wait_ok = true;
                            break;
                        }

                        // Get and show progress
                        NsSystemUpdateProgress update_progress = {};
                        rc = amssu::GetPrepareUpdateProgress(&update_progress);
                        if(R_SUCCEEDED(rc))
                        {
                            this->processBar->SetProgress(static_cast<double>(update_progress.current_size));
                            this->processBar->SetMaxValue(static_cast<double>(update_progress.total_size));
                            global_app->CallForRender();
                        }
                    }
                    else break;
                }
                if(wait_ok)
                {
                    this->processBar->SetVisible(false);
                    this->infoText->SetText(cfg::strings::Main.GetString(427));
                    global_app->CallForRender();
                    rc = amssu::ApplyPreparedUpdate();
                    if(R_SUCCEEDED(rc))
                    {
                        global_app->CreateShowDialog(cfg::strings::Main.GetString(424), cfg::strings::Main.GetString(431) + "\n" + cfg::strings::Main.GetString(432), { cfg::strings::Main.GetString(234) }, true);
                        hos::Reboot();
                    }
                    else global_app->ShowNotification(cfg::strings::Main.GetString(430));
                }
                else global_app->ShowNotification(cfg::strings::Main.GetString(429));
            }
            else global_app->ShowNotification(cfg::strings::Main.GetString(429));
        }
        else global_app->ShowNotification(cfg::strings::Main.GetString(428));

        global_app->CreateShowDialog(cfg::strings::Main.GetString(424), cfg::strings::Main.GetString(432), { cfg::strings::Main.GetString(234) }, true);
        hos::Reboot();
    }
}