
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

#include <ui/ui_InstallLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <iomanip>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    InstallLayout::InstallLayout() : pu::ui::Layout()
    {
        this->installText = pu::ui::elm::TextBlock::New(150, 320, set::GetDictionaryEntry(151));
        this->installText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->installText->SetColor(gsets.CustomScheme.Text);
        this->installBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->installBar);
        this->Add(this->installText);
        this->Add(this->installBar);
    }

    void InstallLayout::StartInstall(pu::String Path, fs::Explorer *Exp, Storage Location, bool OmitConfirmation)
    {
        nsp::Installer inst(Path, Exp, Location);

        auto rc = inst.PrepareInstallation();
        if(rc != 0)
        {
            if(rc == err::Make(err::ErrorDescription::TitleAlreadyInstalled))
            {
                auto sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(272) + "\n" + set::GetDictionaryEntry(273) + "\n" + set::GetDictionaryEntry(274), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                if(sopt == 0)
                {
                    auto title = hos::Locate(inst.GetApplicationId());
                    if(title.ApplicationId == inst.GetApplicationId())
                    {
                        hos::RemoveTitle(title);
                        inst.FinalizeInstallation();
                        auto rc = inst.PrepareInstallation();
                        if(rc != 0)
                        {
                            HandleResult(rc, set::GetDictionaryEntry(251));
                            return;
                        }
                    }
                }
                else
                {
                    return;
                }
            }
            else
            {
                HandleResult(rc, set::GetDictionaryEntry(251));
                return;
            }
        }

        bool doinstall = false;

        if(OmitConfirmation) doinstall = true;
        else
        {
            pu::String info = set::GetDictionaryEntry(82) + "\n\n";
            switch(inst.GetContentMetaType())
            {
                case ncm::ContentMetaType::Application:
                    info += set::GetDictionaryEntry(83);
                    break;
                case ncm::ContentMetaType::Patch:
                    info += set::GetDictionaryEntry(84);
                    break;
                case ncm::ContentMetaType::AddOnContent:
                    info += set::GetDictionaryEntry(85);
                    break;
                default:
                    info += set::GetDictionaryEntry(86);
                    break;
            }
            info += "\n";
            hos::ApplicationIdMask idmask = hos::IsValidApplicationId(inst.GetApplicationId());
            switch(idmask)
            {
                case hos::ApplicationIdMask::Official:
                    info += set::GetDictionaryEntry(87);
                    break;
                case hos::ApplicationIdMask::Homebrew:
                    info += set::GetDictionaryEntry(88);
                    break;
                case hos::ApplicationIdMask::Invalid:
                    info += set::GetDictionaryEntry(89);
                    break;
            }
            info += "\n" + set::GetDictionaryEntry(90) + " " + hos::FormatApplicationId(inst.GetApplicationId());
            info += "\n\n";
            auto NACP = inst.GetNACP();
            if(NACP->version[0] != '\0')
            {
                NacpLanguageEntry *lent;
                nacpGetLanguageEntry(NACP, &lent);
                if(lent == NULL) for(u32 i = 0; i < 16; i++)
                {
                    lent = &NACP->lang[i];
                    if((lent->name[0] != '\0') && (lent->author[0] != '\0')) break;
                }
                info += set::GetDictionaryEntry(91) + " ";
                info += lent->name;
                info += "\n" + set::GetDictionaryEntry(92) + " ";
                info += lent->author;
                info += "\n" + set::GetDictionaryEntry(109) + " ";
                info += NACP->version;
                info += "\n\n";
            }
            auto NCAs = inst.GetNCAs();
            info += set::GetDictionaryEntry(93) + " ";
            for(u32 i = 0; i < NCAs.size(); i++)
            {
                ncm::ContentType t = NCAs[i].Type;
                switch(t)
                {
                    case ncm::ContentType::Control:
                        info += set::GetDictionaryEntry(166);
                        break;
                    case ncm::ContentType::Data:
                        info += set::GetDictionaryEntry(165);
                        break;
                    case ncm::ContentType::LegalInformation:
                        info += set::GetDictionaryEntry(168);
                        break;
                    case ncm::ContentType::Meta:
                        info += set::GetDictionaryEntry(163);
                        break;
                    case ncm::ContentType::OfflineHtml:
                        info += set::GetDictionaryEntry(167);
                        break;
                    case ncm::ContentType::Program:
                        info += set::GetDictionaryEntry(164);
                        break;
                    default:
                        break;
                }
                if(i != (NCAs.size() - 1)) info += ", ";
            }

            u8 kgen = inst.GetKeyGeneration();
            u8 masterkey = kgen - 1;
            info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(kgen) + " ";
            switch(masterkey)
            {
                case 0:
                    info += "(1.0.0 - 2.3.0)";
                    break;
                case 1:
                    info += "(3.0.0)";
                    break;
                case 2:
                    info += "(3.0.1 - 3.0.2)";
                    break;
                case 3:
                    info += "(4.0.0 - 4.1.0)";
                    break;
                case 4:
                    info += "(5.0.0 - 5.1.0)";
                    break;
                case 5:
                    info += "(6.0.0 - 6.1.0)";
                    break;
                case 6:
                    info += "(6.2.0)";
                    break;
                case 7:
                    info += "(7.0.0 - 8.0.1)";
                    break;
                case 8:
                    info += "(8.1.0)";
                    break;
                case 9:
                    info += "(9.0.0)";
                    break;
                default:
                    info += set::GetDictionaryEntry(96);
                    break;
            }

            if(inst.HasTicket())
            {
                auto Tik = inst.GetTicketData();
                info += "\n\n" + set::GetDictionaryEntry(94) + "\n\n";
                info += set::GetDictionaryEntry(235) + " " + Tik.TitleKey;
                info += "\n" + set::GetDictionaryEntry(236) + " ";
                switch(Tik.Signature)
                {
                    case hos::TicketSignature::RSA_4096_SHA1:
                        info += "RSA 4096 (SHA1)";
                        break;
                    case hos::TicketSignature::RSA_2048_SHA1:
                        info += "RSA 2048 (SHA1)";
                        break;
                    case hos::TicketSignature::ECDSA_SHA1:
                        info += "ECDSA (SHA256)";
                        break;
                    case hos::TicketSignature::RSA_4096_SHA256:
                        info += "RSA 4096 (SHA256)";
                        break;
                    case hos::TicketSignature::RSA_2048_SHA256:
                        info += "RSA 2048 (SHA256)";
                        break;
                    case hos::TicketSignature::ECDSA_SHA256:
                        info += "ECDSA (SHA256)";
                        break;
                    default:
                        break;
                }
            }
            else info += "\n\n" + set::GetDictionaryEntry(97);
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), info, { set::GetDictionaryEntry(65), set::GetDictionaryEntry(18) }, true, inst.GetExportedIconPath());

            doinstall = (sopt == 0);
        }
        
        if(doinstall)
        {
            rc = inst.PreProcessContents();
            if(rc != 0)
            {
                HandleResult(rc, set::GetDictionaryEntry(251));
                return;
            }
            this->installText->SetText(set::GetDictionaryEntry(146));
            mainapp->CallForRender();
            this->installBar->SetVisible(true);
            hos::LockAutoSleep();
            rc = inst.WriteContents([&](ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total, u64 BytesSec)
            {
                this->installBar->SetMaxValue(Total);
                pu::String name = set::GetDictionaryEntry(148) + " \'"  + hos::ContentIdAsString(Record.ContentId);
                if(Record.Type == ncm::ContentType::Meta) name += ".cnmt";
                u64 speed = (u64)BytesSec;
                u64 size = (u64)(Total - Done);
                u64 secstime = size / speed;
                name += ".nca\'... (" + fs::FormatSize(BytesSec) + "/s  -  " + hos::FormatTime(secstime) + ")";
                this->installText->SetText(name);
                this->installBar->SetProgress(Done);
                mainapp->CallForRender();
            });
            hos::UnlockAutoSleep();
        }
        this->installBar->SetVisible(false);
        mainapp->CallForRender();
        if(rc != 0) HandleResult(rc, set::GetDictionaryEntry(251));
        else if(doinstall) mainapp->ShowNotification(set::GetDictionaryEntry(150));
    }
}