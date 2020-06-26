
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

#include <ui/ui_InstallLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <iomanip>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    InstallLayout::InstallLayout() : pu::ui::Layout()
    {
        this->installText = pu::ui::elm::TextBlock::New(150, 320, cfg::strings::Main.GetString(151));
        this->installText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->installText->SetColor(global_settings.custom_scheme.Text);
        this->installBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->installBar);
        this->Add(this->installText);
        this->Add(this->installBar);
    }

    void InstallLayout::StartInstall(String Path, fs::Explorer *Exp, Storage Location, bool OmitConfirmation)
    {
        global_app->CreateShowDialog("Ah", Path, { "Q" }, false);
        nsp::Installer inst(Path, Exp, Location);

        global_app->CreateShowDialog("A", "A", { "Q" }, false);

        auto rc = inst.PrepareInstallation();
        if(R_FAILED(rc))
        {
            if(rc == err::result::ResultTitleAlreadyInstalled)
            {
                auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(77), cfg::strings::Main.GetString(272) + "\n" + cfg::strings::Main.GetString(273) + "\n" + cfg::strings::Main.GetString(274), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                if(sopt == 0)
                {
                    auto title = hos::Locate(inst.GetApplicationId());
                    if(title.ApplicationId == inst.GetApplicationId())
                    {
                        hos::RemoveTitle(title);
                        inst.FinalizeInstallation();
                        auto rc = inst.PrepareInstallation();
                        if(R_FAILED(rc))
                        {
                            HandleResult(rc, cfg::strings::Main.GetString(251));
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
                HandleResult(rc, cfg::strings::Main.GetString(251));
                return;
            }
        }

        bool doinstall = false;

        if(OmitConfirmation) doinstall = true;
        else
        {
            String info = cfg::strings::Main.GetString(82) + "\n\n";
            switch(inst.GetContentMetaType())
            {
                case ncm::ContentMetaType::Application:
                    info += cfg::strings::Main.GetString(83);
                    break;
                case ncm::ContentMetaType::Patch:
                    info += cfg::strings::Main.GetString(84);
                    break;
                case ncm::ContentMetaType::AddOnContent:
                    info += cfg::strings::Main.GetString(85);
                    break;
                default:
                    info += cfg::strings::Main.GetString(86);
                    break;
            }
            info += "\n";
            hos::ApplicationIdMask idmask = hos::IsValidApplicationId(inst.GetApplicationId());
            switch(idmask)
            {
                case hos::ApplicationIdMask::Official:
                    info += cfg::strings::Main.GetString(87);
                    break;
                case hos::ApplicationIdMask::Homebrew:
                    info += cfg::strings::Main.GetString(88);
                    break;
                case hos::ApplicationIdMask::Invalid:
                    info += cfg::strings::Main.GetString(89);
                    break;
            }
            info += "\n" + cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(inst.GetApplicationId());
            info += "\n\n";
            auto NACP = inst.GetNACP();
            if(NACP->display_version[0] != '\0')
            {
                NacpLanguageEntry *lent;
                nacpGetLanguageEntry(NACP, &lent);
                if(lent == nullptr) for(u32 i = 0; i < 16; i++)
                {
                    lent = &NACP->lang[i];
                    if((lent->name[0] != '\0') && (lent->author[0] != '\0')) break;
                }
                info += cfg::strings::Main.GetString(91) + " ";
                info += lent->name;
                info += "\n" + cfg::strings::Main.GetString(92) + " ";
                info += lent->author;
                info += "\n" + cfg::strings::Main.GetString(109) + " ";
                info += NACP->display_version;
                info += "\n\n";
            }
            auto NCAs = inst.GetNCAs();
            info += cfg::strings::Main.GetString(93) + " ";
            for(u32 i = 0; i < NCAs.size(); i++)
            {
                ncm::ContentType t = NCAs[i].Type;
                switch(t)
                {
                    case ncm::ContentType::Control:
                        info += cfg::strings::Main.GetString(166);
                        break;
                    case ncm::ContentType::Data:
                        info += cfg::strings::Main.GetString(165);
                        break;
                    case ncm::ContentType::LegalInformation:
                        info += cfg::strings::Main.GetString(168);
                        break;
                    case ncm::ContentType::Meta:
                        info += cfg::strings::Main.GetString(163);
                        break;
                    case ncm::ContentType::OfflineHtml:
                        info += cfg::strings::Main.GetString(167);
                        break;
                    case ncm::ContentType::Program:
                        info += cfg::strings::Main.GetString(164);
                        break;
                    default:
                        break;
                }
                if(i != (NCAs.size() - 1)) info += ", ";
            }

            u8 kgen = inst.GetKeyGeneration();
            u8 masterkey = kgen - 1;
            info += "\n" + cfg::strings::Main.GetString(95) + " " + std::to_string(kgen) + " ";
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
                    info += cfg::strings::Main.GetString(96);
                    break;
            }

            if(inst.HasTicket())
            {
                auto ticket = inst.GetTicketFile();
                info += "\n\n" + cfg::strings::Main.GetString(94) + "\n\n";
                info += cfg::strings::Main.GetString(235) + " " + ticket.GetTitleKey();
                info += "\n" + cfg::strings::Main.GetString(236) + " ";
                switch(ticket.signature)
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
            else info += "\n\n" + cfg::strings::Main.GetString(97);
            int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(77), info, { cfg::strings::Main.GetString(65), cfg::strings::Main.GetString(18) }, true, inst.GetExportedIconPath());

            doinstall = (sopt == 0);
        }
        
        if(doinstall)
        {
            rc = inst.PreProcessContents();
            if(R_FAILED(rc))
            {
                HandleResult(rc, cfg::strings::Main.GetString(251));
                return;
            }
            this->installText->SetText(cfg::strings::Main.GetString(146));
            global_app->CallForRender();
            this->installBar->SetVisible(true);
            hos::LockAutoSleep();
            rc = inst.WriteContents([&](ncm::ContentRecord Record, u32 Content, u32 ContentCount, double Done, double Total, u64 BytesSec)
            {
                this->installBar->SetMaxValue(Total);
                String name = cfg::strings::Main.GetString(148) + " \'"  + hos::ContentIdAsString(Record.ContentId);
                if(Record.Type == ncm::ContentType::Meta) name += ".cnmt";
                u64 speed = (u64)BytesSec;
                u64 size = (u64)(Total - Done);
                u64 secstime = size / speed;
                name += ".nca\'... (" + fs::FormatSize(BytesSec) + "/s  -  " + hos::FormatTime(secstime) + ")";
                this->installText->SetText(name);
                this->installBar->SetProgress(Done);
                global_app->CallForRender();
            });
            hos::UnlockAutoSleep();
        }
        this->installBar->SetVisible(false);
        global_app->CallForRender();
        if(R_FAILED(rc)) HandleResult(rc, cfg::strings::Main.GetString(251));
        else if(doinstall) global_app->ShowNotification(cfg::strings::Main.GetString(150));
    }
}