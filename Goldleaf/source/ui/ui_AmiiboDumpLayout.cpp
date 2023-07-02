
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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
        this->info_txt->SetText(cfg::Strings.GetString(294));
        auto rc = nfp::Initialize();
        if(R_SUCCEEDED(rc)) {
            this->info_txt->SetText(cfg::Strings.GetString(295));
            while(!nfp::IsReady()) {
                g_MainApplication->CallForRender();
            }
            rc = nfp::Open();
            if(R_SUCCEEDED(rc)) {
                const auto tag = nfp::GetTagInfo();
                const auto model = nfp::GetModelInfo();
                const auto common = nfp::GetCommonInfo();
                const auto reg = nfp::GetRegisterInfo();
                const auto data = nfp::GetAll();

                const auto option = g_MainApplication->CreateShowDialog(cfg::Strings.GetString(283), cfg::Strings.GetString(317) + " '" + reg.amiibo_name + "'?", { cfg::Strings.GetString(111), cfg::Strings.GetString(112) }, true);
                if(option == 0) {
                    this->info_txt->SetText(cfg::Strings.GetString(296) + " '" + reg.amiibo_name + "' " + cfg::Strings.GetString(297));
                    const auto virtual_amiibo_folder = nfp::ExportAsVirtualAmiibo(tag, reg, common, model, data);
                    g_MainApplication->ShowNotification("'" + std::string(reg.amiibo_name) + "' " + cfg::Strings.GetString(298) + " (" + virtual_amiibo_folder + ")");
                }
                nfp::Close();
            }
            nfp::Exit();
        }

        if(R_FAILED(rc)) {
            HandleResult(rc, cfg::Strings.GetString(456));
        }
    }

}