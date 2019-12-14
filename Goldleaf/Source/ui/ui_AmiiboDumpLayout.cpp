
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

#include <ui/ui_AmiiboDumpLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref mainapp;
extern set::Settings gsets;

namespace ui
{
    AmiiboDumpLayout::AmiiboDumpLayout() : pu::ui::Layout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, "-");
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->infoText);
    }

    void AmiiboDumpLayout::StartDump()
    {
        this->infoText->SetText(set::GetDictionaryEntry(294));
        auto rc = nfp::Initialize();
        if(rc == 0)
        {
            this->infoText->SetText(set::GetDictionaryEntry(295));
            while(!nfp::IsReady())
            {
                mainapp->CallForRender();
            }
            auto rc = nfp::Open();
            if(rc == 0)
            {
                auto tag = nfp::GetTagInfo();
                auto model = nfp::GetModelInfo();
                auto common = nfp::GetCommonInfo();
                auto reg = nfp::GetRegisterInfo();

                auto name = String(reg.amiibo_name);
                auto sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(283), set::GetDictionaryEntry(317) + " '" + name + "'?", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112) }, true);
                if(sopt == 0)
                {
                    this->infoText->SetText(set::GetDictionaryEntry(296) + " '" + name + "' " + set::GetDictionaryEntry(297));
                    nfp::DumpToEmuiibo(tag, reg, common, model);
                    mainapp->ShowNotification("'" + name + "' " + set::GetDictionaryEntry(298));
                }
                nfp::Close();
            }
        }
        nfp::Finalize();
    }
}