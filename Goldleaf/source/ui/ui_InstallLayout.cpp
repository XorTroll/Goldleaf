
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

#include <ui/ui_InstallLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        std::string FormatContentType(const NcmContentType type) {
            switch(type) {
                case NcmContentType_Meta: {
                    return cfg::strings::Main.GetString(163);
                }
                case NcmContentType_Program: {
                    return cfg::strings::Main.GetString(164);
                }
                case NcmContentType_Data: {
                    return cfg::strings::Main.GetString(165);
                }
                case NcmContentType_Control: {
                    return cfg::strings::Main.GetString(166);
                }
                case NcmContentType_HtmlDocument: {
                    return cfg::strings::Main.GetString(167);
                }
                case NcmContentType_LegalInformation: {
                    return cfg::strings::Main.GetString(168);
                }
                default: {
                    return "<unk>";
                }
            }
        }

    }

    InstallLayout::InstallLayout() : pu::ui::Layout() {
        u32 cur_y = 180;
        this->speed_info_text = pu::ui::elm::TextBlock::New(0, cur_y, "A");
        this->speed_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->speed_info_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->speed_info_text);
        cur_y += this->speed_info_text->GetHeight() + 25;
        this->speed_info_text->SetVisible(false);

        for(u32 i = 0; i < ncm::ContentTypeCount; i++) {
            auto info_text = pu::ui::elm::TextBlock::New(0, cur_y, "A");
            info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
            info_text->SetColor(g_Settings.custom_scheme.text);
            cur_y += info_text->GetHeight() + 10;
            info_text->SetVisible(false);
            auto p_bar = pu::ui::elm::ProgressBar::New(0, cur_y, 600, 30, 0.0f);
            p_bar->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
            g_Settings.ApplyProgressBarColor(p_bar);
            cur_y += p_bar->GetHeight() + 15;
            p_bar->SetVisible(false);

            this->content_info_texts.push_back(info_text);
            this->content_p_bars.push_back(p_bar);
            this->Add(info_text);
            this->Add(p_bar);
        }
    }

    void InstallLayout::StartInstall(const std::string &path, fs::Explorer *exp, const NcmStorageId storage_id, const bool omit_confirmation) {
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
                        rc = nsp_installer.PrepareInstallation();
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
                info += FormatContentType(static_cast<NcmContentType>(cnt.content_type)) + ", ";
            }
            info.pop_back();
            info.pop_back();

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
                    info += "(9.0.0 - 9.0.1)";
                    break;
                }
                case 10: {
                    info += "(9.1.0 - 12.0.3)";
                    break;
                }
                case 11: {
                    info += "(12.1.0)";
                    break;
                }
                case 12: {
                    info += "(13.0.0 - 13.2.1)";
                    break;
                }
                case 13: {
                    info += "(14.0.0 -)";
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
                info += cfg::strings::Main.GetString(235) + " " + ticket.data.GetTitleKey();
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
            else {
                info += "\n\n" + cfg::strings::Main.GetString(97);
            }

            const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(77), info, { cfg::strings::Main.GetString(65), cfg::strings::Main.GetString(18) }, true, nsp_installer.GetExportedIconPath());
            do_install = option == 0;
        }
        
        if(do_install) {
            rc = nsp_installer.StartInstallation();
            if(R_FAILED(rc)) {
                HandleResult(rc, cfg::strings::Main.GetString(251));
                return;
            }

            hos::LockAutoSleep();
            const auto t1 = std::chrono::steady_clock::now();

            auto last_tp = std::chrono::steady_clock::now();
            auto first_time = true;
            this->speed_info_text->SetVisible(true);
            for(u32 i = 0; i < nsp_installer.GetContents().size(); i++) {
                this->content_info_texts.at(i)->SetVisible(true);
                this->content_p_bars.at(i)->SetVisible(true);
            }
            rc = nsp_installer.WriteContents([&](const nsp::ContentWriteProgress &write_progress) {
                const auto cur_tp = std::chrono::steady_clock::now();
                const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                last_tp = cur_tp;
                // By elapsed time and written bytes, compute how much data has been written in 1 second
                const auto speed_bps = (1000.0f / time_diff) * (double)(write_progress.written_size);

                size_t cur_size = 0;
                size_t total_size = 0;
                u32 i = 0;
                for(const auto &[type, entry] : write_progress.entries) {
                    const double progress = 100.0f * (((double)entry.cur_offset) / ((double)entry.size));
                    const auto text = FormatContentType(type) + " (" + fs::FormatSize(entry.size) + ")";
                    cur_size += entry.cur_offset;
                    total_size += entry.size;

                    this->content_info_texts.at(i)->SetText(text);
                    this->content_p_bars.at(i)->SetProgress((double)entry.cur_offset);
                    this->content_p_bars.at(i)->SetMaxProgress((double)entry.size);
                    i++;
                }

                const auto speed_text = "Speed: " + fs::FormatSize(speed_bps) + "/s, ETA: " + hos::FormatTime((u64)((1.0f / speed_bps) * (double)(total_size - cur_size)));
                this->speed_info_text->SetText(speed_text);

                g_MainApplication->CallForRender();
            });
            const auto t2 = std::chrono::steady_clock::now();
            GLEAF_WARN_FMT("Elapsed time: %f s", ((double)(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) / 1000.0));
            hos::UnlockAutoSleep();
        }
        this->speed_info_text->SetVisible(false);
        for(u32 i = 0; i < ncm::ContentTypeCount; i++) {
            this->content_info_texts.at(i)->SetVisible(false);
            this->content_p_bars.at(i)->SetVisible(false);
        }
        g_MainApplication->CallForRender();

        if(R_FAILED(rc)) {
            HandleResult(rc, cfg::strings::Main.GetString(251));
        }
        else if(do_install) {
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(150));
        }
    }

}