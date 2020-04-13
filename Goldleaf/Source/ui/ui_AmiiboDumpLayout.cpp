
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

#include <ui/ui_AmiiboDumpLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    AmiiboDumpLayout::AmiiboDumpLayout() : pu::ui::Layout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, "-");
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(global_settings.custom_scheme.Text);
        this->Add(this->infoText);
    }

    void AmiiboDumpLayout::StartDump()
    {
        this->infoText->SetText(cfg::strings::Main.GetString(294));
        auto rc = nfp::Initialize();
        if(R_SUCCEEDED(rc))
        {
            this->infoText->SetText(cfg::strings::Main.GetString(295));
            while(!nfp::IsReady())
            {
                global_app->CallForRender();
            }
            auto rc = nfp::Open();
            if(R_SUCCEEDED(rc))
            {
                auto tag = nfp::GetTagInfo();
                auto model = nfp::GetModelInfo();
                auto common = nfp::GetCommonInfo();
                auto reg = nfp::GetRegisterInfo();

                auto name = String(reg.amiibo_name);
                auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(283), cfg::strings::Main.GetString(317) + " '" + name + "'?", { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112) }, true);
                if(sopt == 0)
                {
                    this->infoText->SetText(cfg::strings::Main.GetString(296) + " '" + name + "' " + cfg::strings::Main.GetString(297));
                    nfp::DumpToEmuiibo(tag, reg, common, model);
                    global_app->ShowNotification("'" + name + "' " + cfg::strings::Main.GetString(298));
                }
                nfp::Close();
            }
        }
        nfp::Exit();
    }
}