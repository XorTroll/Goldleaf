
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
        this->exp_text = pu::ui::elm::TextBlock::New(150, 320, cfg::Strings.GetString(151));
        this->exp_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->exp_text->SetColor(g_Settings.custom_scheme.text);
        this->cnt_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->cnt_p_bar->SetVisible(false);
        g_Settings.ApplyProgressBarColor(this->cnt_p_bar);
        this->Add(this->exp_text);
        this->Add(this->cnt_p_bar);
    }

    void ContentExportLayout::StartExport(hos::Title &cnt, const bool has_tik) {
        EnsureDirectories();
        g_MainApplication->CallForRender();

        const auto format_app_id = hos::FormatApplicationId(cnt.app_id);
        
        auto sd_exp = fs::GetSdCardExplorer();
        const auto out_dir = sd_exp->MakeAbsolute(GLEAF_PATH_EXPORT_TITLE_DIR "/" + format_app_id);
        sd_exp->CreateDirectory(out_dir);
        this->exp_text->SetText(cfg::Strings.GetString(192));
        g_MainApplication->CallForRender();
        if(has_tik) {
            expt::ExportTicketCert(cnt.app_id, true);
        }
        this->exp_text->SetText(cfg::Strings.GetString(193));
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

        hos::LockAutoSleep();
        if(cnt.storage_id == NcmStorageId_SdCard) {
            this->exp_text->SetText(cfg::Strings.GetString(194));

            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto out_cnt_path = out_dir + "/" + hos::ContentIdAsString(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
                fs::CreateConcatenationFile(out_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                expt::DecryptCopyNAX0ToNCA(&cnt_storage, cnt_id, out_cnt_path, [&](const double done, const double total) {
                    this->cnt_p_bar->SetMaxProgress(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
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
                hos::UnlockAutoSleep();
                HandleResult(rc::goldleaf::ResultCouldNotLocateTitleContents, cfg::Strings.GetString(198));
                g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
                return;
            }

            this->exp_text->SetText(cfg::Strings.GetString(195));

            for(const auto &[cnt_is_meta, cnt_id] : export_cnts) {
                const auto cnt_ncm_path = expt::GetContentIdPath(&cnt_storage, cnt_id);
                const auto cnt_nand_path = nand_exp->FullPathFor("Contents/" + cnt_ncm_path.substr(__builtin_strlen("@UserContent://")));
                const auto out_cnt_path = out_dir + "/" + hos::ContentIdAsString(cnt_id) + (cnt_is_meta ? ".cnmt" : "") + ".nca";
                fs::CreateConcatenationFile(out_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(cnt_nand_path, out_cnt_path, [&](const double done, const double total) {
                    this->cnt_p_bar->SetMaxProgress(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
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
        this->cnt_p_bar->SetVisible(true);
        this->exp_text->SetText(cfg::Strings.GetString(196));
        const auto ok = nsp::GenerateFrom(out_dir, out_nsp, [&](const u64 done, const u64 total) {
            this->cnt_p_bar->SetMaxProgress((double)total);
            this->cnt_p_bar->SetProgress((double)done);
            g_MainApplication->CallForRender();
        });
        hos::UnlockAutoSleep();
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