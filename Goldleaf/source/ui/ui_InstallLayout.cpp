
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
                    return cfg::Strings.GetString(163);
                }
                case NcmContentType_Program: {
                    return cfg::Strings.GetString(164);
                }
                case NcmContentType_Data: {
                    return cfg::Strings.GetString(165);
                }
                case NcmContentType_Control: {
                    return cfg::Strings.GetString(166);
                }
                case NcmContentType_HtmlDocument: {
                    return cfg::Strings.GetString(167);
                }
                case NcmContentType_LegalInformation: {
                    return cfg::Strings.GetString(168);
                }
                default: {
                    return "<unk>";
                }
            }
        }

        std::string FormatTicketFlags(const cnt::TicketFlags flags) {
            std::string fmt = "";

            #define _GLEAF_TICKET_FMT_HANDLE_FLAG(flag) \
            if(static_cast<bool>(flags & cnt::TicketFlags::flag)) { \
                if(!fmt.empty()) { \
                    fmt += ", ";    \
                } \
                fmt += #flag; \
            }

            _GLEAF_TICKET_FMT_HANDLE_FLAG(PreInstalled);
            _GLEAF_TICKET_FMT_HANDLE_FLAG(Shared);
            _GLEAF_TICKET_FMT_HANDLE_FLAG(AllContents);
            _GLEAF_TICKET_FMT_HANDLE_FLAG(DeviceLinkIndependent);
            _GLEAF_TICKET_FMT_HANDLE_FLAG(Temporary);

            if(fmt.empty()) {
                fmt = "Default";
            }
            return fmt;
        }

        enum class InstallDialogResult {
            Cancel,
            Prev,
            Next,
            Install
        };

        InstallDialogResult ShowInstallDialogPage(nsp::Installer &nsp_installer, const size_t program_idx) {
            const size_t program_count = nsp_installer.GetInstallablePrograms().size();
            const auto &program = nsp_installer.GetInstallablePrograms().at(program_idx);

            auto info = cfg::Strings.GetString(82) + "\n\n";

            const auto &cnts = nsp_installer.GetContents();
            info += cfg::Strings.GetString(93) + " ";
            u32 cnt_count = 0;
            for(const auto &cnt: cnts) {
                info += FormatContentType(static_cast<NcmContentType>(cnt.content_type)) + ", ";
                cnt_count++;
                if(cnt_count > 3) {
                    cnt_count = 0;
                    info += "\n";
                }
            }
            if(info.back() == '\n') {
                info.pop_back();
            }
            info.pop_back();
            info.pop_back();

            const auto key_gen = nsp_installer.GetKeyGeneration();
            info += "\n" + cfg::Strings.GetString(95) + " " + std::to_string(key_gen) + " ";
            info += "(" + hos::GetKeyGenerationRange(key_gen) + ")";

            if(nsp_installer.HasTicket()) {
                const auto &ticket = nsp_installer.GetTicketFile();
                info += "\n\n" + cfg::Strings.GetString(94) + "\n";
                info += "[" + FormatTicketFlags(ticket.data.flags) + "]";
                info += "\n - " + cfg::Strings.GetString(235) + " " + ticket.data.GetTitleKey();
                info += "\n - " + cfg::Strings.GetString(236) + " ";
                switch(ticket.signature) {
                    case cnt::TicketSignature::RSA_4096_SHA1: {
                        info += "RSA-4096 PKCS#1 v1.5 (SHA1)";
                        break;
                    }
                    case cnt::TicketSignature::RSA_2048_SHA1: {
                        info += "RSA-2048 PKCS#1 v1.5 (SHA1)";
                        break;
                    }
                    case cnt::TicketSignature::ECDSA_SHA1: {
                        info += "ECDSA (SHA256)";
                        break;
                    }
                    case cnt::TicketSignature::RSA_4096_SHA256: {
                        info += "RSA-4096 PKCS#1 v1.5 (SHA256)";
                        break;
                    }
                    case cnt::TicketSignature::RSA_2048_SHA256: {
                        info += "RSA-2048 PKCS#1 v1.5 (SHA256)";
                        break;
                    }
                    case cnt::TicketSignature::ECDSA_SHA256: {
                        info += "ECDSA (SHA256)";
                        break;
                    }
                    case cnt::TicketSignature::HMAC_SHA1_160: {
                        info += "HMAC-SHA1-160";
                        break;
                    }
                        
                    default: {
                        break;
                    }
                }
            }
            else {
                info += "\n\n" + cfg::Strings.GetString(97);
            }

            info += "\n\n";
            if(program_count > 1) {
                info += cfg::Strings.GetString(461) + " (" + std::to_string(program_idx + 1) + " / " + std::to_string(program_count) + "):\n";
            }
            else {
                info += cfg::Strings.GetString(462) + "\n";
            }
            switch(program.GetContentMetaType()) {
                case NcmContentMetaType_Application: {
                    info += cfg::Strings.GetString(83);
                    break;
                }
                case NcmContentMetaType_Patch: {
                    info += cfg::Strings.GetString(84);
                    break;
                }
                case NcmContentMetaType_AddOnContent: {
                    info += cfg::Strings.GetString(85);
                    break;
                }
                default: {
                    info += cfg::Strings.GetString(86);
                    break;
                }
            }
            info += "\n";
            if(!cnt::IsApplicationNacpEmpty(program.nacp_data)) {
                info += " - " + cfg::Strings.GetString(90) + " ";
                info += util::FormatApplicationId(program.meta_key.id);
                info += "\n - " + cfg::Strings.GetString(91) + " ";
                info += cnt::FindApplicationNacpName(program.nacp_data);
                info += "\n - " + cfg::Strings.GetString(92) + " ";
                info += cnt::FindApplicationNacpAuthor(program.nacp_data);
                info += "\n - " + cfg::Strings.GetString(109) + " ";
                info += program.nacp_data.display_version;
            }

            if(program_idx == 0) {
                if(program_count > 1) {
                    const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), info, { cfg::Strings.GetString(65), cfg::Strings.GetString(465), cfg::Strings.GetString(18) }, true, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(program.icon_path)));
                    switch(option) {
                        case 0:
                            return InstallDialogResult::Install;
                        case 1:
                            return InstallDialogResult::Next;
                        default:
                            return InstallDialogResult::Cancel;
                    }
                }
                else {
                    const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), info, { cfg::Strings.GetString(65), cfg::Strings.GetString(18) }, true, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(program.icon_path)));
                    switch(option) {
                        case 0:
                            return InstallDialogResult::Install;
                        default:
                            return InstallDialogResult::Cancel;
                    }
                }
            }
            else if(program_idx == (program_count - 1)) {
                const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), info, { cfg::Strings.GetString(65), cfg::Strings.GetString(464), cfg::Strings.GetString(18) }, true, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(program.icon_path)));
                switch(option) {
                    case 0:
                        return InstallDialogResult::Install;
                    case 1:
                        return InstallDialogResult::Prev;
                    default:
                        return InstallDialogResult::Cancel;
                }
            }
            else {
                const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), info, { cfg::Strings.GetString(65), cfg::Strings.GetString(464), cfg::Strings.GetString(465), cfg::Strings.GetString(18) }, true, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(program.icon_path)));
                switch(option) {
                    case 0:
                        return InstallDialogResult::Install;
                    case 1:
                        return InstallDialogResult::Prev;
                    case 2:
                        return InstallDialogResult::Next;
                    default:
                        return InstallDialogResult::Cancel;
                }
            }
        }

        void HandleInstallationFailure(const Result rc, nsp::Installer &nsp_installer) {
            for(const auto &program: nsp_installer.GetInstallablePrograms()) {
                const auto program_id = program.meta_key.id;
                auto program_app = cnt::ExistsApplicationContent(program_id, static_cast<NcmContentMetaType>(program.meta_key.type));
                if(program_app.has_value()) {
                    GLEAF_LOG_FMT("Removing program %016lX failed installation leftovers...", program_id);

                    const auto &cnt_status = program_app.value().get().meta_status_list;
                    const auto cnt_it = std::find_if(cnt_status.begin(), cnt_status.end(), [&](const NsApplicationContentMetaStatus &cnt_status) -> bool {
                        return cnt_status.application_id == program_id;
                    });
                    if(cnt_it != cnt_status.end()) {
                        GLEAF_LOG_FMT("Removing program %016lX...", program_id);
                        const auto cnt_idx = std::distance(cnt_status.begin(), cnt_it);
                        cnt::RemoveApplicationContentById(program_app.value().get(), cnt_idx);
                    }
                }
            }

            HandleResult(rc, cfg::Strings.GetString(251));
        }

    }

    InstallLayout::InstallLayout() : pu::ui::Layout() {
        this->speed_info_text = pu::ui::elm::TextBlock::New(0, 320, "...");
        this->speed_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->speed_info_text->SetColor(g_Settings.GetColorScheme().text);
    }

    void InstallLayout::StartInstall(const std::string &path, const std::string &pres_path, fs::Explorer *exp, const NcmStorageId storage_id, const bool omit_confirmation) {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(77), CommonIconKind::Storage, cfg::Strings.GetString(145) + " " + pres_path);
        ScopeGuard on_exit([&]() {
            // Just in case
            cnt::NotifyApplicationsChanged();
            g_MainApplication->GetApplicationListLayout()->NotifyApplicationsChanged();
            g_MainApplication->ReturnToParentLayout();
        });

        nsp::Installer nsp_installer(path, exp, storage_id);

        auto rc = nsp_installer.PrepareInstallation();
        if(R_FAILED(rc)) {
            if(rc == rc::goldleaf::ResultContentAlreadyInstalled) {
                const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), cfg::Strings.GetString(272) + "\n" + cfg::Strings.GetString(273) + "\n" + cfg::Strings.GetString(274), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                if(option == 0) {
                    for(const auto &program: nsp_installer.GetInstallablePrograms()) {
                        const auto program_id = program.meta_key.id;
                        GLEAF_WARN_FMT("Checking program %016lX...", program_id);
                        const auto program_app = cnt::ExistsApplicationContent(program_id, static_cast<NcmContentMetaType>(program.meta_key.type));
                        if(program_app.has_value()) {
                            GLEAF_WARN_FMT("Removing program %016lX...", program_id);
                            const auto &cnt_status = program_app.value().get().meta_status_list;
                            const auto cnt_it = std::find_if(cnt_status.begin(), cnt_status.end(), [&](const NsApplicationContentMetaStatus &cnt_status) -> bool {
                                return cnt_status.application_id == program_id;
                            });
                            if(cnt_it != cnt_status.end()) {
                                GLEAF_WARN_FMT("Removing program %016lX actually...", program_id);
                                const auto cnt_idx = std::distance(cnt_status.begin(), cnt_it);
                                cnt::RemoveApplicationContentById(program_app.value().get(), cnt_idx);
                            }
                        }
                    }
                    nsp_installer.FinalizeInstallation();
                    rc = nsp_installer.PrepareInstallation();
                    if(R_FAILED(rc)) {
                        HandleResult(rc, cfg::Strings.GetString(251));
                        return;
                    }
                }
                else {
                    return;
                }
            }
            else  {
                HandleResult(rc, cfg::Strings.GetString(251));
                return;
            }
        }

        auto do_install = false;
        if(omit_confirmation) {
            do_install = true;
        }
        else {
            const auto program_count = nsp_installer.GetInstallablePrograms().size();
            size_t program_i = 0;
            while(program_i < program_count) {
                const auto dialog_res = ShowInstallDialogPage(nsp_installer, program_i);
                switch(dialog_res) {
                    case InstallDialogResult::Prev: {
                        program_i--;
                        break;
                    }
                    case InstallDialogResult::Next: {
                        program_i++;
                        break;
                    }
                    case InstallDialogResult::Install: {
                        do_install = true;
                        program_i = SIZE_MAX;
                        break;
                    }
                    default: {
                        do_install = false;
                        program_i = SIZE_MAX;
                        break;
                    }
                }
            }
        }
        
        if(do_install) {
            rc = nsp_installer.InstallTicketCertificate();
            if(R_FAILED(rc)) {
                // Nothing has been installed yet, so no need to rollback
                return;
            }

            hos::LockExit();

            g_MainApplication->ClearLayout(g_MainApplication->GetInstallLayout());
            this->content_info_texts.clear();
            this->content_p_bars.clear();
            u32 cur_y = this->speed_info_text->GetY();
            this->Add(this->speed_info_text);
            cur_y += this->speed_info_text->GetHeight() + 25;

            constexpr auto cnts_per_column = 4;
            const auto &cnts = nsp_installer.GetContents();
            const auto column_count = (cnts.size() + 3) / 4;
            constexpr auto margin = 75;
            const auto p_bar_width = (pu::ui::render::ScreenWidth - (column_count + 1) * margin) / column_count;

            const auto base_column_y = cur_y;
            auto cur_x = margin;
            u32 j = 0;
            for(u32 i = 0; i < cnts.size(); i++) {
                auto info_text = pu::ui::elm::TextBlock::New(cur_x, cur_y, "A");
                info_text->SetColor(g_Settings.GetColorScheme().text);
                cur_y += info_text->GetHeight() + 15;
                info_text->SetVisible(false);
                auto p_bar = pu::ui::elm::ProgressBar::New(cur_x, cur_y, p_bar_width, 40, 0.0f);
                p_bar->SetProgressColor(g_Settings.GetColorScheme().progress_bar);
                p_bar->SetBackgroundColor(g_Settings.GetColorScheme().progress_bar_bg);
                cur_y += p_bar->GetHeight() + 25;
                p_bar->SetVisible(false);

                this->content_info_texts.push_back(info_text);
                this->content_p_bars.push_back(p_bar);
                this->Add(info_text);
                this->Add(p_bar);

                j++;
                if(j >= cnts_per_column) {
                    j = 0;
                    cur_x += p_bar_width + margin;
                    cur_y = base_column_y;
                }
            }

            auto last_tp = std::chrono::steady_clock::now();

            rc = nsp_installer.WriteContents([&](const nsp::ContentWriteProgress &write_start) {
                u32 i = 0;
                u32 cnt_counts[cnt::MaxContentCount] = {};
                for(const auto &entry : write_start.entries) {
                    const u32 cnt_id = static_cast<u32>(entry.type);
                    const auto text = FormatContentType(entry.type) + ((cnt_counts[cnt_id] > 0) ? (" " + std::to_string(cnt_counts[cnt_id])) : "") + " (" + fs::FormatSize(entry.size) + ")";
                    cnt_counts[cnt_id]++;

                    this->content_info_texts.at(i)->SetText(text);
                    this->content_info_texts.at(i)->SetVisible(true);
                    this->content_p_bars.at(i)->SetMaxProgress((double)entry.size);
                    this->content_p_bars.at(i)->SetVisible(true);
                    i++;
                }
                this->speed_info_text->SetVisible(true);

                g_MainApplication->CallForRender();
            }, [&](const nsp::ContentWriteProgress &write_progress) {
                const auto cur_tp = std::chrono::steady_clock::now();
                const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                last_tp = cur_tp;
                // By elapsed time and written bytes, compute how much data has been written in 1 second
                const auto speed_bps = (1000.0f / time_diff) * (double)(write_progress.written_size);

                size_t cur_size = 0;
                size_t total_size = 0;
                u32 i = 0;
                for(const auto &entry : write_progress.entries) {
                    cur_size += entry.cur_offset;
                    total_size += entry.size;

                    this->content_p_bars.at(i)->SetProgress((double)entry.cur_offset);
                    i++;
                }

                const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + util::FormatTime((u64)((1.0f / speed_bps) * (double)(total_size - cur_size)));
                this->speed_info_text->SetText(speed_text);

                g_MainApplication->CallForRender();
            });

            rc = nsp_installer.UpdateRecordAndContentMetas();
            if(R_FAILED(rc)) {
                HandleInstallationFailure(rc, nsp_installer);
                return;
            }

            hos::UnlockExit();
        }

        if(R_FAILED(rc)) {
            HandleInstallationFailure(rc, nsp_installer);
            return;
        }
        else if(do_install) {
            g_MainApplication->ShowNotification(cfg::Strings.GetString(150));

            if(g_Settings.json_settings.installs.value().show_deletion_prompt_after_install.value()) {
                g_MainApplication->GetBrowserLayout()->PromptDeleteFile(path);
            }
        }
    }

}
