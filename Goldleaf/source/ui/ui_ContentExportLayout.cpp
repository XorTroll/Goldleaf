
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

    This program_cnt_id is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program_cnt_id is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program_cnt_id.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_ContentExportLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <expt/expt_Export.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    ContentExportLayout::ContentExportLayout() {
        this->speed_info_text = pu::ui::elm::TextBlock::New(0, 180, "A");
        this->speed_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->speed_info_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->speed_info_text);
    }

    void ContentExportLayout::StartExport(hos::Title &cnt, const bool has_tik) {
        EnsureDirectories();
        g_MainApplication->CallForRender();

        g_MainApplication->ClearLayout(g_MainApplication->GetContentExportLayout());
        this->content_info_texts.clear();
        this->content_p_bars.clear();
        this->Add(this->speed_info_text);

        const auto format_app_id = hos::FormatApplicationId(cnt.app_id);
        
        auto sd_exp = fs::GetSdCardExplorer();
        const auto out_dir = sd_exp->MakeAbsolute(GLEAF_PATH_EXPORT_TITLE_DIR "/" + format_app_id);
        sd_exp->CreateDirectory(out_dir);
        this->speed_info_text->SetText(cfg::Strings.GetString(192));
        g_MainApplication->CallForRender();
        if(has_tik) {
            expt::ExportTicketCert(cnt.app_id, true);
        }
        this->speed_info_text->SetText(cfg::Strings.GetString(193));
        g_MainApplication->CallForRender();
    
        NcmContentStorage cnt_storage;
        auto rc = ncmOpenContentStorage(&cnt_storage, cnt.storage_id);
        if(R_FAILED(rc)) {
            HandleResult(rc::goldleaf::ResultCouldNotLocateTitleContents, cfg::Strings.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }
        ScopeGuard on_exit_1([&]() {
            ncmContentStorageClose(&cnt_storage);
        });

        NcmContentMetaDatabase cnt_meta_db;
        rc = ncmOpenContentMetaDatabase(&cnt_meta_db, cnt.storage_id);
        if(R_FAILED(rc)) {
            HandleResult(rc::goldleaf::ResultCouldNotLocateTitleContents, cfg::Strings.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }
        ScopeGuard on_exit_2([&]() {
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        });

        std::vector<std::pair<bool, NcmContentId>> export_cnts;
        auto is_any_meta = false;
        for(const auto &cnts: cnt.title_cnts) {
            for(u32 i = 0; i < ncm::ContentTypeCount; i++) {
                if(!cnts.cnts[i].is_empty) {
                    const auto is_meta = static_cast<NcmContentType>(i) == NcmContentType_Meta;
                    export_cnts.push_back({ is_meta, cnts.cnts[i].id });
                    if(is_meta) {
                        is_any_meta = true;
                    }
                }
            }
        }
        if(!is_any_meta) {
            HandleResult(rc::goldleaf::ResultCouldNotLocateTitleContents, cfg::Strings.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }

        u32 cur_y = 180 + this->speed_info_text->GetHeight() + 25;

        constexpr auto cnts_per_column = 4;
        const auto total_count = export_cnts.size() + 1;
        const auto column_count = (total_count + 3) / 4;
        constexpr auto margin = 25;
        const auto p_bar_width = (pu::ui::render::ScreenWidth - (column_count + 1) * margin) / column_count;

        const auto base_column_y = cur_y;
        auto cur_x = margin;
        u32 j = 0;
        for(u32 i = 0; i < total_count; i++) {
            auto info_text = pu::ui::elm::TextBlock::New(cur_x, cur_y, "A");
            if(i < export_cnts.size()) {
                const auto cnt_name = hos::ContentIdAsString(export_cnts.at(i).second) + (export_cnts.at(i).first ? ".cnmt" : "") + ".nca";
                info_text->SetText(cnt_name);
            }
            else {
                info_text->SetText(cfg::Strings.GetString(196));
            }
            info_text->SetColor(g_Settings.custom_scheme.text);
            cur_y += info_text->GetHeight() + 10;
            auto p_bar = pu::ui::elm::ProgressBar::New(cur_x, cur_y, p_bar_width, 30, 0.0f);
            g_Settings.ApplyProgressBarColor(p_bar);
            cur_y += p_bar->GetHeight() + 15;

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

        hos::LockExit();
        auto last_tp = std::chrono::steady_clock::now();
        if(cnt.storage_id == NcmStorageId_SdCard) {
            u32 i = 0;
            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto cnt_name = hos::ContentIdAsString(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
                const auto out_cnt_path = out_dir + "/" + cnt_name;
                fs::CreateConcatenationFile(out_cnt_path);
                const auto rc = expt::DecryptCopyNAX0ToNCA(&cnt_storage, cnt_id, out_cnt_path, [&](const double file_size) {
                    this->content_p_bars.at(i)->SetMaxProgress(file_size);
                }, [&](const double cur_rw_size) {
                    const auto cur_tp = std::chrono::steady_clock::now();
                    const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                    last_tp = cur_tp;

                    const auto speed_bps = (1000.0f / time_diff) * cur_rw_size;
                    const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + hos::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(i)->GetMaxProgress() - this->content_p_bars.at(i)->GetProgress())));
                    this->speed_info_text->SetText(speed_text);

                    this->content_p_bars.at(i)->IncrementProgress(cur_rw_size);
                    g_MainApplication->CallForRender();
                });
                if(R_FAILED(rc)) {
                    HandleResult(rc, cfg::Strings.GetString(198));
                    return;
                }
                i++;
            }
        }
        else {
            fs::Explorer *nand_exp = nullptr;
            if(cnt.storage_id == NcmStorageId_BuiltInSystem) {
                nand_exp = fs::GetNANDSystemExplorer();
            }
            else if(cnt.storage_id == NcmStorageId_BuiltInUser) {
                nand_exp = fs::GetNANDUserExplorer();
            }
            else {
                hos::UnlockExit();
                HandleResult(rc::goldleaf::ResultCouldNotLocateTitleContents, cfg::Strings.GetString(198));
                g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
                return;
            }

            u32 i = 0;
            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto cnt_name = hos::ContentIdAsString(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
                const auto cnt_ncm_path = expt::GetContentIdPath(&cnt_storage, cnt_id);
                const auto cnt_nand_path = nand_exp->FullPathFor("Contents/" + cnt_ncm_path.substr(__builtin_strlen("@UserContent://")));
                const auto out_cnt_path = out_dir + "/" + cnt_name;
                fs::CreateConcatenationFile(out_cnt_path);
                fs::CopyFileProgress(cnt_nand_path, out_cnt_path, [&](const size_t file_size) {
                    this->content_p_bars.at(i)->SetMaxProgress(file_size);
                }, [&](const size_t cur_rw_size) {
                    const auto cur_tp = std::chrono::steady_clock::now();
                    const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                    last_tp = cur_tp;

                    const auto speed_bps = (1000.0f / time_diff) * cur_rw_size;
                    const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + hos::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(i)->GetMaxProgress() - this->content_p_bars.at(i)->GetProgress())));
                    this->speed_info_text->SetText(speed_text);

                    this->content_p_bars.at(i)->IncrementProgress(cur_rw_size);
                    g_MainApplication->CallForRender();
                });
                i++;
            }
        }

        auto out_nsp = sd_exp->MakeAbsolute(GLEAF_PATH_EXPORT_TITLE_DIR "/");
        if(cnt.TryGetNacp()) {
            out_nsp += hos::FindNacpName(cnt.nacp) + " [" + format_app_id + "] [v" + std::to_string(cnt.version) + "].nsp";
        }
        else {
            out_nsp += format_app_id + ".nsp";
        }

        fs::CreateConcatenationFile(out_nsp);
        const auto nsp_i = export_cnts.size();
        const auto ok = nsp::GenerateFrom(out_dir, out_nsp, [&](const size_t file_size) {
            this->content_p_bars.at(nsp_i)->SetMaxProgress(file_size);
        }, [&](const size_t cur_rw_size) {
            const auto cur_tp = std::chrono::steady_clock::now();
            const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
            last_tp = cur_tp;

            const auto speed_bps = (1000.0f / time_diff) * cur_rw_size;
            const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + hos::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(nsp_i)->GetMaxProgress() - this->content_p_bars.at(nsp_i)->GetProgress())));
            this->speed_info_text->SetText(speed_text);

            this->content_p_bars.at(nsp_i)->IncrementProgress(cur_rw_size);
            g_MainApplication->CallForRender();
        });
        hos::UnlockExit();
        sd_exp->EmptyDirectory(GLEAF_PATH_EXPORT_TEMP_DIR);
        sd_exp->DeleteDirectory(out_dir);
        if(ok) {
            g_MainApplication->ShowNotification(cfg::Strings.GetString(197) + " '" + out_nsp + "'");
        }
        else {
            HandleResult(rc::goldleaf::ResultCouldNotBuildNsp, cfg::Strings.GetString(198));
            sd_exp->EmptyDirectory(GLEAF_PATH_EXPORT_DIR);
            EnsureDirectories();
        }
    }

}