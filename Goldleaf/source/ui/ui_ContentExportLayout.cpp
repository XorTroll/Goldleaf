
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

    namespace {

        std::string SanitizeExportName(const std::string &name) {
            auto sanitized_name = name;
            for(auto &c: sanitized_name) {
                if(c == '/' || c == '\\' || c == ':') {
                    c = '_';
                }
            }
            return sanitized_name;
        }

    }

    ContentExportLayout::ContentExportLayout() {
        this->speed_info_text = pu::ui::elm::TextBlock::New(0, 290, "A");
        this->speed_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->speed_info_text->SetColor(g_Settings.GetColorScheme().text);
        this->Add(this->speed_info_text);
    }

    void ContentExportLayout::StartExport(const cnt::Application &app, const u32 cnt_i, const bool has_tik) {
        const auto &cnt_status = app.meta_status_list.at(cnt_i);
        const auto &cnts = app.contents.at(cnt_i);
        const auto cnt_storage_id = static_cast<NcmStorageId>(cnt_status.storageID);
        const auto format_app_id = util::FormatApplicationId(cnt_status.application_id);
        
        g_MainApplication->LoadMenuData(true, cfg::Strings.GetString(505), GetApplicationIcon(app.record.id), cfg::Strings.GetString(359) + ": " + app.cache.display_name + " (" + cnt::GetContentMetaTypeName(static_cast<NcmContentMetaType>(cnt_status.meta_type)) + ", " + format_app_id + ")");

        ScopeGuard on_exit([&]() {
            g_MainApplication->ReturnToParentLayout();
        });

        EnsureDirectories();
        g_MainApplication->CallForRender();

        g_MainApplication->ClearLayout(g_MainApplication->GetContentExportLayout());
        this->content_info_texts.clear();
        this->content_p_bars.clear();
        this->Add(this->speed_info_text);
        
        auto sd_exp = fs::GetSdCardExplorer();
        const auto out_dir = sd_exp->MakeAbsolute(GLEAF_PATH_EXPORT_TITLE_DIR "/" + format_app_id);
        sd_exp->CreateDirectory(out_dir);
        this->speed_info_text->SetText(cfg::Strings.GetString(192));
        g_MainApplication->CallForRender();
        if(has_tik) {
            expt::ExportTicketCert(cnt_status.application_id, true);
        }
        this->speed_info_text->SetText(cfg::Strings.GetString(193));
        g_MainApplication->CallForRender();
    
        NcmContentStorage cnt_storage;
        auto rc = ncmOpenContentStorage(&cnt_storage, cnt_storage_id);
        if(R_FAILED(rc)) {
            HandleResult(rc::goldleaf::ResultUnableToLocateContents, cfg::Strings.GetString(198));
            return;
        }
        ScopeGuard on_exit_1([&]() {
            ncmContentStorageClose(&cnt_storage);
        });

        NcmContentMetaDatabase cnt_meta_db;
        rc = ncmOpenContentMetaDatabase(&cnt_meta_db, cnt_storage_id);
        if(R_FAILED(rc)) {
            HandleResult(rc::goldleaf::ResultUnableToLocateContents, cfg::Strings.GetString(198));
            return;
        }
        ScopeGuard on_exit_2([&]() {
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        });

        std::vector<std::pair<bool, NcmContentId>> export_cnts;
        auto is_any_meta = false;
        for(u32 i = 0; i < cnt::MaxContentCount; i++) {
            if(cnts.cnt_ids[i].has_value()) {
                const auto is_meta = static_cast<NcmContentType>(i) == NcmContentType_Meta;
                export_cnts.push_back({ is_meta, cnts.cnt_ids[i].value() });
                if(is_meta) {
                    is_any_meta = true;
                }
            }
        }
        if(!is_any_meta) {
            HandleResult(rc::goldleaf::ResultUnableToLocateContents, cfg::Strings.GetString(198));
            return;
        }

        u32 cur_y = this->speed_info_text->GetY() + this->speed_info_text->GetHeight() + 35;

        constexpr auto cnts_per_column = 4;
        const auto total_count = export_cnts.size() + 1;
        const auto column_count = (total_count + 3) / 4;
        constexpr auto margin = 45;
        const auto p_bar_width = (pu::ui::render::ScreenWidth - (column_count + 1) * margin) / column_count;

        const auto base_column_y = cur_y;
        auto cur_x = margin;
        u32 j = 0;
        for(u32 i = 0; i < total_count; i++) {
            auto info_text = pu::ui::elm::TextBlock::New(cur_x, cur_y, "A");
            if(i < export_cnts.size()) {
                const auto cnt_name = util::FormatContentId(export_cnts.at(i).second) + (export_cnts.at(i).first ? ".cnmt" : "") + ".nca";
                info_text->SetText(cnt_name);
            }
            else {
                info_text->SetText(cfg::Strings.GetString(196));
            }
            info_text->SetColor(g_Settings.GetColorScheme().text);
            cur_y += info_text->GetHeight() + 20;
            auto p_bar = pu::ui::elm::ProgressBar::New(cur_x, cur_y, p_bar_width, 30, 0.0f);
            p_bar->SetProgressColor(g_Settings.GetColorScheme().progress_bar);
            p_bar->SetBackgroundColor(g_Settings.GetColorScheme().progress_bar_bg);
            cur_y += p_bar->GetHeight() + 20;

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
        if(cnt_storage_id == NcmStorageId_SdCard) {
            u32 i = 0;
            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto cnt_name = util::FormatContentId(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
                const auto out_cnt_path = out_dir + "/" + cnt_name;
                fs::CreateConcatenationFile(out_cnt_path);
                const auto rc = expt::DecryptCopyNax0ToNca(&cnt_storage, cnt_id, out_cnt_path, [&](const double file_size) {
                    this->content_p_bars.at(i)->SetMaxProgress(file_size);
                }, [&](const double cur_rw_size) {
                    const auto cur_tp = std::chrono::steady_clock::now();
                    const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                    last_tp = cur_tp;

                    const auto speed_bps = (1000.0f / time_diff) * cur_rw_size;
                    const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + util::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(i)->GetMaxProgress() - this->content_p_bars.at(i)->GetProgress())));
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
            if(cnt_storage_id == NcmStorageId_BuiltInSystem) {
                nand_exp = fs::GetNANDSystemExplorer();
            }
            else if(cnt_storage_id == NcmStorageId_BuiltInUser) {
                nand_exp = fs::GetNANDUserExplorer();
            }
            else {
                hos::UnlockExit();
                HandleResult(rc::goldleaf::ResultUnableToLocateContents, cfg::Strings.GetString(198));
                return;
            }

            u32 i = 0;
            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto cnt_name = util::FormatContentId(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
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
                    const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + util::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(i)->GetMaxProgress() - this->content_p_bars.at(i)->GetProgress())));
                    this->speed_info_text->SetText(speed_text);

                    this->content_p_bars.at(i)->IncrementProgress(cur_rw_size);
                    g_MainApplication->CallForRender();
                });
                i++;
            }
        }

        auto out_nsp = sd_exp->MakeAbsolute(GLEAF_PATH_EXPORT_TITLE_DIR "/");
        u32 max_version = 0;
        for(const auto &status: app.meta_status_list) {
            if(status.version > max_version) {
                max_version = status.version;
            }
        }
        out_nsp += SanitizeExportName(app.cache.display_name) + " [" + format_app_id + "] [v" + std::to_string(max_version) + "].nsp";

        fs::CreateConcatenationFile(out_nsp);
        const auto nsp_i = export_cnts.size();
        const auto ok = nsp::GenerateFrom(out_dir, out_nsp, [&](const size_t file_size) {
            this->content_p_bars.at(nsp_i)->SetMaxProgress(file_size);
        }, [&](const size_t cur_rw_size) {
            const auto cur_tp = std::chrono::steady_clock::now();
            const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
            last_tp = cur_tp;

            const auto speed_bps = (1000.0f / time_diff) * cur_rw_size;
            const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + util::FormatTime((u64)((1.0f / speed_bps) * (this->content_p_bars.at(nsp_i)->GetMaxProgress() - this->content_p_bars.at(nsp_i)->GetProgress())));
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
            HandleResult(rc::goldleaf::ResultUnableToBuildNsp, cfg::Strings.GetString(198));
            sd_exp->EmptyDirectory(GLEAF_PATH_EXPORT_DIR);
            EnsureDirectories();
        }
    }

}
