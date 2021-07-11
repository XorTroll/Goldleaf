
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    InstallLayout::InstallLayout() : pu::ui::Layout() {
        this->install_text = pu::ui::elm::TextBlock::New(150, 320, cfg::strings::Main.GetString(151));
        this->install_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->install_text->SetColor(g_Settings.custom_scheme.Text);
        this->install_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->install_p_bar);
        this->Add(this->install_text);
        this->Add(this->install_p_bar);
    }

    void InstallLayout::StartInstall(String path, fs::Explorer *exp, NcmStorageId storage_id, bool omit_confirmation) {
        nsp::Installer nsp_installer(path, exp, storage_id);

        auto rc = nsp_installer.PrepareInstallation();
        if(R_FAILED(rc)) {
            if(rc == err::result::ResultTitleAlreadyInstalled) {
                const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(77), cfg::strings::Main.GetString(272) + "\n" + cfg::strings::Main.GetString(273) + "\n" + cfg::strings::Main.GetString(274), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                if(option == 0) {
                    const auto title = hos::Locate(nsp_installer.GetApplicationId());
                    if(title.app_id == nsp_installer.GetApplicationId()) {
                        hos::RemoveTitle(title);
                        nsp_installer.FinalizeInstallation();
                        auto rc = nsp_installer.PrepareInstallation();
                        if(R_FAILED(rc)) {
                            HandleResult(rc, cfg::strings::Main.GetString(251));
                            return;
                        }
                    }
                }
                else {
                    return;
                }
            }
            else  {
                HandleResult(rc, cfg::strings::Main.GetString(251));
                return;
            }
        }

        auto do_install = false;
        if(omit_confirmation) {
            do_install = true;
        }
        else {
            auto info = cfg::strings::Main.GetString(82) + "\n\n";
            switch(nsp_installer.GetContentMetaType()) {
                case NcmContentMetaType_Application: {
                    info += cfg::strings::Main.GetString(83);
                    break;
                }
                case NcmContentMetaType_Patch: {
                    info += cfg::strings::Main.GetString(84);
                    break;
                }
                case NcmContentMetaType_AddOnContent: {
                    info += cfg::strings::Main.GetString(85);
                    break;
                }
                default: {
                    info += cfg::strings::Main.GetString(86);
                    break;
                }
            }
            info += "\n";
            const auto app_id_mask = hos::GetApplicationIdMask(nsp_installer.GetApplicationId());
            switch(app_id_mask) {
                case hos::ApplicationIdMask::Official: {
                    info += cfg::strings::Main.GetString(87);
                    break;
                }
                case hos::ApplicationIdMask::Homebrew: {
                    info += cfg::strings::Main.GetString(88);
                    break;
                }
                case hos::ApplicationIdMask::Invalid: {
                    info += cfg::strings::Main.GetString(89);
                    break;
                }
            }
            info += "\n" + cfg::strings::Main.GetString(90) + " " + hos::FormatApplicationId(nsp_installer.GetApplicationId());
            info += "\n\n";
            const auto &nacp = nsp_installer.GetNacp();
            if(!hos::IsNacpEmpty(nacp)) {
                info += cfg::strings::Main.GetString(91) + " ";
                info += hos::FindNacpName(nacp);
                info += "\n" + cfg::strings::Main.GetString(92) + " ";
                info += hos::FindNacpAuthor(nacp);
                info += "\n" + cfg::strings::Main.GetString(109) + " ";
                info += nacp.display_version;
                info += "\n\n";
            }
            const auto cnts = nsp_installer.GetContents();
            info += cfg::strings::Main.GetString(93) + " ";
            for(const auto &cnt: cnts) {
                switch(static_cast<NcmContentType>(cnt.content_type)) {
                    case NcmContentType_Meta: {
                        info += cfg::strings::Main.GetString(163);
                        break;
                    }
                    case NcmContentType_Program: {
                        info += cfg::strings::Main.GetString(164);
                        break;
                    }
                    case NcmContentType_Data: {
                        info += cfg::strings::Main.GetString(165);
                        break;
                    }
                    case NcmContentType_Control: {
                        info += cfg::strings::Main.GetString(166);
                        break;
                    }
                    case NcmContentType_HtmlDocument: {
                        info += cfg::strings::Main.GetString(167);
                        break;
                    }
                    case NcmContentType_LegalInformation: {
                        info += cfg::strings::Main.GetString(168);
                        break;
                    }
                    default: {
                        break;
                    }
                }
                info += ", ";
            }
            info = info.substr(0, info.length() - 2);

            const auto keygen = nsp_installer.GetKeyGeneration();
            const auto m_key = keygen - 1;
            info += "\n" + cfg::strings::Main.GetString(95) + " " + std::to_string(keygen) + " ";
            switch(m_key) {
                case 0: {
                    info += "(1.0.0 - 2.3.0)";
                    break;
                }
                case 1: {
                    info += "(3.0.0)";
                    break;
                }
                case 2: {
                    info += "(3.0.1 - 3.0.2)";
                    break;
                }
                case 3: {
                    info += "(4.0.0 - 4.1.0)";
                    break;
                }
                case 4: {
                    info += "(5.0.0 - 5.1.0)";
                    break;
                }
                case 5: {
                    info += "(6.0.0 - 6.1.0)";
                    break;
                }
                case 6: {
                    info += "(6.2.0)";
                    break;
                }
                case 7: {
                    info += "(7.0.0 - 8.0.1)";
                    break;
                }
                case 8: {
                    info += "(8.1.0 - 8.1.1)";
                    break;
                }
                case 9: {
                    info += "(9.0.0 - 9.2.0)";
                    break;
                }
                case 10: {
                    info += "(10.0.0 -)";
                    break;
                }
                // TODO (high priority): more keys?
                default: {
                    info += cfg::strings::Main.GetString(96);
                    break;
                }
            }

            if(nsp_installer.HasTicket()) {
                const auto ticket = nsp_installer.GetTicketFile();
                info += "\n\n" + cfg::strings::Main.GetString(94) + "\n\n";
                info += cfg::strings::Main.GetString(235) + " " + ticket.GetTitleKey();
                info += "\n" + cfg::strings::Main.GetString(236) + " ";
                switch(ticket.signature) {
                    case hos::TicketSignature::RSA_4096_SHA1: {
                        info += "RSA 4096 (SHA1)";
                        break;
                    }
                    case hos::TicketSignature::RSA_2048_SHA1: {
                        info += "RSA 2048 (SHA1)";
                        break;
                    }
                    case hos::TicketSignature::ECDSA_SHA1: {
                        info += "ECDSA (SHA256)";
                        break;
                    }
                    case hos::TicketSignature::RSA_4096_SHA256: {
                        info += "RSA 4096 (SHA256)";
                        break;
                    }
                    case hos::TicketSignature::RSA_2048_SHA256: {
                        info += "RSA 2048 (SHA256)";
                        break;
                    }
                    case hos::TicketSignature::ECDSA_SHA256: {
                        info += "ECDSA (SHA256)";
                        break;
                    }
                        
                    default: {
                        break;
                    }
                }
            }
            else info += "\n\n" + cfg::strings::Main.GetString(97);

            const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(77), info, { cfg::strings::Main.GetString(65), cfg::strings::Main.GetString(18) }, true, nsp_installer.GetExportedIconPath());
            do_install = option == 0;
        }
        
        if(do_install) {
            rc = nsp_installer.PreProcessContents();
            if(R_FAILED(rc)) {
                HandleResult(rc, cfg::strings::Main.GetString(251));
                return;
            }
            this->install_text->SetText(cfg::strings::Main.GetString(146));
            g_MainApplication->CallForRender();
            this->install_p_bar->SetVisible(true);
            hos::LockAutoSleep();
            rc = nsp_installer.WriteContents([&](NcmContentInfo cnt_info, u32 cnt, u32 cnt_count, double done, double total, u64 bytes_per_sec) {
                this->install_p_bar->SetMaxValue(total);
                auto name = cfg::strings::Main.GetString(148) + " \'"  + hos::ContentIdAsString(cnt_info.content_id);
                if(cnt_info.content_type == NcmContentType_Meta) {
                    name += ".cnmt";
                }
                u64 size = (u64)(total - done);
                const auto secs = size / bytes_per_sec;
                name += ".nca\'... (" + fs::FormatSize(bytes_per_sec) + "/s  →  " + hos::FormatTime(secs) + ")";
                this->install_text->SetText(name);
                this->install_p_bar->SetProgress(done);
                g_MainApplication->CallForRender();
            });
            hos::UnlockAutoSleep();
        }
        this->install_p_bar->SetVisible(false);
        g_MainApplication->CallForRender();

        if(R_FAILED(rc)) {
            HandleResult(rc, cfg::strings::Main.GetString(251));
        }
        else if(do_install) {
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(150));
        }
    }

}