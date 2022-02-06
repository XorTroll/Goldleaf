
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

#include <ui/ui_AmiiboDumpLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <nfp/nfp_Amiibo.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    AmiiboDumpLayout::AmiiboDumpLayout() : pu::ui::Layout() {
        this->info_txt = pu::ui::elm::TextBlock::New(150, 320, "-");
        this->info_txt->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->info_txt->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->info_txt);
    }

    void AmiiboDumpLayout::StartDump() {
        this->info_txt->SetText(cfg::strings::Main.GetString(294));
        auto rc = nfp::Initialize();
        if(R_SUCCEEDED(rc)) {
            this->info_txt->SetText(cfg::strings::Main.GetString(295));
            while(!nfp::IsReady()) {
                g_MainApplication->CallForRender();
            }
            rc = nfp::Open();
            if(R_SUCCEEDED(rc)) {
                const auto tag = nfp::GetTagInfo();
                const auto model = nfp::GetModelInfo();
                const auto common = nfp::GetCommonInfo();
                const auto reg = nfp::GetRegisterInfo();

                const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(283), cfg::strings::Main.GetString(317) + " '" + reg.amiibo_name + "'?", { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112) }, true);
                if(option == 0) {
                    this->info_txt->SetText(cfg::strings::Main.GetString(296) + " '" + reg.amiibo_name + "' " + cfg::strings::Main.GetString(297));
                    nfp::DumpToEmuiibo(tag, reg, common, model);
                    g_MainApplication->ShowNotification("'" + std::string(reg.amiibo_name) + "' " + cfg::strings::Main.GetString(298));
                }
                nfp::Close();
            }
            nfp::Exit();
        }
        // TODO: if(R_FAILED(rc)) HandleResult(rc, "Amiibo:");
    }

}