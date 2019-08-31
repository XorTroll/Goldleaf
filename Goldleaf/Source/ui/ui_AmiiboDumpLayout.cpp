#include <ui/ui_AmiiboDumpLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

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

                auto name = pu::String(reg.amiibo_name);
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