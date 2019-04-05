#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    InstallLayout::InstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 320, set::GetDictionaryEntry(151));
        this->installText->SetTextAlign(pu::element::TextAlign::CenterHorizontal);
        this->installText->SetColor(gsets.CustomScheme.Text);
        this->installBar = new pu::element::ProgressBar(340, 360, 600, 30);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void InstallLayout::StartInstall(nsp::Installer *Inst, pu::Layout *Prev, bool Delete, std::string Input, std::string PInput)
    {
        if(IsInstalledTitle()) appletBeginBlockingHomeButton(0);
        mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " \'" + PInput + "\'");
        this->installText->SetText(set::GetDictionaryEntry(146));
        mainapp->CallForRender();
        Result rc = Inst->ProcessRecords();
        if(rc != 0)
        {
            int sopt = -1;
            if(rc == err::Make(err::ErrorDescription::TitleAlreadyInstalled))
            {
                sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(272) + "\n" + set::GetDictionaryEntry(273) + "\n" + set::GetDictionaryEntry(274), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112) }, true);
                if(sopt == 0)
                {
                    horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                    if(t.ApplicationId == 0) sopt = -1;
                    else
                    {
                        Result rc2 = horizon::RemoveTitle(t);
                        if(rc2 == 0)
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(275), { set::GetDictionaryEntry(234) }, true);
                            mainapp->LoadLayout(Prev);
                            return;
                        }
                        else HandleResult(rc2, set::GetDictionaryEntry(247));
                        if(rc != 0) sopt = -1;
                    }
                }
            }
            if(sopt != 0)
            {
                if(IsInstalledTitle()) appletEndBlockingHomeButton();
                delete Inst;
                Inst = NULL;
                if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
                {
                    horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                    horizon::RemoveTitle(t);
                }
                HandleResult(rc, set::GetDictionaryEntry(251));
                mainapp->LoadLayout(Prev);
                return;
            }
            rc = Inst->ProcessRecords();
            if(rc != 0)
            {
                if(IsInstalledTitle()) appletEndBlockingHomeButton();
                delete Inst;
                Inst = NULL;
                if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
                {
                    horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                    horizon::RemoveTitle(t);
                }
                HandleResult(rc, set::GetDictionaryEntry(251));
                mainapp->LoadLayout(Prev);
                return;
            }
        }
        this->installText->SetText(set::GetDictionaryEntry(147));
        mainapp->CallForRender();
        rc = Inst->WriteContents([&](ncm::ContentRecord NCA, u32 Index, u32 Count, int Percentage)
        {
            std::string name = set::GetDictionaryEntry(148) + " \'"  + horizon::GetStringFromNCAId(NCA.NCAId);
            if(NCA.Type == ncm::ContentType::Meta) name += ".cnmt";
            name += ".nca\'... (NCA " + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
            this->installText->SetText(name);
            this->installBar->SetProgress((u8)Percentage);
            mainapp->CallForRender();
        });
        if(rc != 0)
        {
            if(IsInstalledTitle()) appletEndBlockingHomeButton();
            delete Inst;
            Inst = NULL;
            if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
            {
                horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                horizon::RemoveTitle(t);
            }
            HandleResult(rc, set::GetDictionaryEntry(251));
            mainapp->LoadLayout(Prev);
            return;
        }
        if(IsInstalledTitle()) appletEndBlockingHomeButton();
        delete Inst;
        Inst = NULL;
        if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(150));
        else
        {
            if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
            {
                horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                horizon::RemoveTitle(t);
            }
            HandleResult(rc, set::GetDictionaryEntry(251));
        }
        mainapp->LoadLayout(Prev);
    }
}