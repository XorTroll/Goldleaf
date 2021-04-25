
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <ui/ui_TitleDumperLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    TitleDumperLayout::TitleDumperLayout() {
        this->dump_text = pu::ui::elm::TextBlock::New(150, 320, cfg::strings::Main.GetString(151));
        this->dump_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->dump_text->SetColor(g_Settings.custom_scheme.Text);
        this->cnt_p_bar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->cnt_p_bar->SetVisible(false);
        g_Settings.ApplyProgressBarColor(this->cnt_p_bar);
        this->Add(this->dump_text);
        this->Add(this->cnt_p_bar);
    }

    void TitleDumperLayout::StartDump(const hos::Title &cnt, bool has_tik) {
        EnsureDirectories();
        g_MainApplication->CallForRender();

        const auto storage_id = static_cast<NcmStorageId>(cnt.location);
        auto format_app_id = hos::FormatApplicationId(cnt.app_id);
        
        auto sd_exp = fs::GetSdCardExplorer();
        auto out_dir = sd_exp->MakeAbsolute(consts::Root + "/dump/title/" + format_app_id);
        sd_exp->CreateDirectory(out_dir);
        this->dump_text->SetText(cfg::strings::Main.GetString(192));
        g_MainApplication->CallForRender();
        if(has_tik) {
            dump::GenerateTicketCert(cnt.app_id);
        }
        this->dump_text->SetText(cfg::strings::Main.GetString(193));
        g_MainApplication->CallForRender();
    
        NcmContentStorage cnt_storage;
        auto rc = ncmOpenContentStorage(&cnt_storage, storage_id);
        if(R_FAILED(rc)) {
            HandleResult(err::result::ResultCouldNotLocateTitleContents, cfg::strings::Main.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }
        NcmContentMetaDatabase cnt_meta_db;
        rc = ncmOpenContentMetaDatabase(&cnt_meta_db, storage_id);
        if(R_FAILED(rc)) {
            ncmContentStorageClose(&cnt_storage);
            HandleResult(err::result::ResultCouldNotLocateTitleContents, cfg::strings::Main.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }

        NcmContentId meta_cnt_id;
        const auto has_meta_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_Meta, &meta_cnt_id);
        if(!has_meta_cnt) {
            ncmContentStorageClose(&cnt_storage);
            ncmContentMetaDatabaseClose(&cnt_meta_db);
            HandleResult(err::result::ResultCouldNotLocateTitleContents, cfg::strings::Main.GetString(198));
            g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
            return;
        }
        auto meta_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &meta_cnt_id);

        NcmContentId program_cnt_id;
        const auto has_program_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_Program, &program_cnt_id);
        String program_cnt_ncm_path;
        if(has_program_cnt) {
            program_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &program_cnt_id);
        }

        NcmContentId control_cnt_id;
        const auto has_control_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_Control, &control_cnt_id);
        String control_cnt_ncm_path;
        if(has_control_cnt) {
            control_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &control_cnt_id);
        }

        NcmContentId legal_info_cnt_id;
        const auto has_legal_info_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_LegalInformation, &legal_info_cnt_id);
        String legal_info_cnt_ncm_path;
        if(has_legal_info_cnt) {
            legal_info_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &legal_info_cnt_id);
        }

        NcmContentId html_doc_cnt_id;
        const auto has_html_doc_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_HtmlDocument, &html_doc_cnt_id);
        String html_doc_cnt_ncm_path;
        if(has_html_doc_cnt) {
            html_doc_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &html_doc_cnt_id);
        }

        NcmContentId data_cnt_id;
        const auto has_data_cnt = dump::GetContentId(&cnt_meta_db, &cnt.meta_key, cnt.app_id, NcmContentType_Data, &data_cnt_id);
        String data_cnt_ncm_path;
        if(has_data_cnt) {
            data_cnt_ncm_path = dump::GetContentIdPath(&cnt_storage, &data_cnt_id);
        }

        hos::LockAutoSleep();
        if(storage_id == NcmStorageId_SdCard) {
            this->dump_text->SetText(cfg::strings::Main.GetString(194));
            auto out_meta_cnt_path = out_dir + "/" + hos::ContentIdAsString(meta_cnt_id) + ".cnmt.nca";
            fs::CreateConcatenationFile(out_meta_cnt_path);
            this->cnt_p_bar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cnt_storage, meta_cnt_id, out_meta_cnt_path, [&](double done, double total) {
                this->cnt_p_bar->SetMaxValue(total);
                this->cnt_p_bar->SetProgress(done);
                g_MainApplication->CallForRender();
            });
            this->cnt_p_bar->SetVisible(false);
            if(has_program_cnt) {
                auto out_program_cnt_path = out_dir + "/" + hos::ContentIdAsString(program_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_program_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cnt_storage, program_cnt_id, out_program_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_control_cnt) {
                auto out_control_cnt_path = out_dir + "/" + hos::ContentIdAsString(control_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_control_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cnt_storage, control_cnt_id, out_control_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_legal_info_cnt) {
                auto out_legal_info_cnt_path = out_dir + "/" + hos::ContentIdAsString(legal_info_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_legal_info_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cnt_storage, legal_info_cnt_id, out_legal_info_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_html_doc_cnt) {
                auto out_html_doc_cnt_path = out_dir + "/" + hos::ContentIdAsString(html_doc_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_html_doc_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cnt_storage, html_doc_cnt_id, out_html_doc_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_data_cnt) {
                auto out_data_cnt_path = out_dir + "/" + hos::ContentIdAsString(data_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_data_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cnt_storage, data_cnt_id, out_data_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
        }
        else {
            fs::Explorer *nand_exp = nullptr;
            if(storage_id == NcmStorageId_BuiltInSystem) {
                nand_exp = fs::GetNANDSystemExplorer();
            }
            else if(storage_id == NcmStorageId_BuiltInUser) {
                nand_exp = fs::GetNANDUserExplorer();
            }
            else {
                hos::UnlockAutoSleep();
                ncmContentStorageClose(&cnt_storage);
                ncmContentMetaDatabaseClose(&cnt_meta_db);
                HandleResult(err::result::ResultCouldNotLocateTitleContents, cfg::strings::Main.GetString(198));
                g_MainApplication->LoadLayout(g_MainApplication->GetContentManagerLayout());
                return;
            }
            this->dump_text->SetText(cfg::strings::Main.GetString(195));

            auto meta_cnt_nand_path = nand_exp->FullPathFor("Contents/" + meta_cnt_ncm_path.substr(15));
            auto out_meta_cnt_path = out_dir + "/" + hos::ContentIdAsString(meta_cnt_id) + ".cnmt.nca";
            fs::CreateConcatenationFile(out_meta_cnt_path);
            this->cnt_p_bar->SetVisible(true);
            fs::CopyFileProgress(meta_cnt_nand_path, out_meta_cnt_path, [&](double done, double total) {
                this->cnt_p_bar->SetMaxValue(total);
                this->cnt_p_bar->SetProgress(done);
                g_MainApplication->CallForRender();
            });
            this->cnt_p_bar->SetVisible(false);

            if(has_program_cnt) {
                auto program_cnt_nand_path = nand_exp->FullPathFor("Contents/" + program_cnt_ncm_path.substr(15));
                auto out_program_cnt_path = out_dir + "/" + hos::ContentIdAsString(program_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_program_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(program_cnt_nand_path, out_program_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }

            if(has_control_cnt) {
                auto control_cnt_nand_path = nand_exp->FullPathFor("Contents/" + control_cnt_ncm_path.substr(15));
                auto out_control_cnt_path = out_dir + "/" + hos::ContentIdAsString(control_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_control_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(control_cnt_nand_path, out_control_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_legal_info_cnt) {
                auto legal_info_cnt_nand_path = nand_exp->FullPathFor("Contents/" + legal_info_cnt_ncm_path.substr(15));
                auto out_legal_info_cnt_path = out_dir + "/" + hos::ContentIdAsString(legal_info_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_legal_info_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(legal_info_cnt_nand_path, out_legal_info_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_html_doc_cnt) {
                auto html_doc_cnt_nand_path = nand_exp->FullPathFor("Contents/" + html_doc_cnt_ncm_path.substr(15));
                auto out_html_doc_cnt_path = out_dir + "/" + hos::ContentIdAsString(html_doc_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_html_doc_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(html_doc_cnt_nand_path, out_html_doc_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
            if(has_data_cnt) {
                auto data_cnt_nand_path = nand_exp->FullPathFor("Contents/" + data_cnt_ncm_path.substr(15));
                auto out_data_cnt_path = out_dir + "/" + hos::ContentIdAsString(data_cnt_id) + ".nca";
                fs::CreateConcatenationFile(out_data_cnt_path);
                this->cnt_p_bar->SetVisible(true);
                fs::CopyFileProgress(data_cnt_nand_path, out_data_cnt_path, [&](double done, double total) {
                    this->cnt_p_bar->SetMaxValue(total);
                    this->cnt_p_bar->SetProgress(done);
                    g_MainApplication->CallForRender();
                });
                this->cnt_p_bar->SetVisible(false);
            }
        }

        // TODO: better name?
        auto out_nsp = "sdmc:/" + consts::Root + "/dump/title/" + format_app_id + ".nsp";
        fs::CreateConcatenationFile(out_nsp);
        this->cnt_p_bar->SetVisible(true);
        this->dump_text->SetText(cfg::strings::Main.GetString(196));
        const auto ok = nsp::GenerateFrom(out_dir, out_nsp, [&](u64 done, u64 total) {
            this->cnt_p_bar->SetMaxValue((double)total);
            this->cnt_p_bar->SetProgress((double)done);
            g_MainApplication->CallForRender();
        });
        hos::UnlockAutoSleep();
        sd_exp->DeleteDirectory(consts::Root + "/dump/temp");
        sd_exp->DeleteDirectory(out_dir);
        if(ok) {
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(197) + " '" + out_nsp + "'");
        }
        else {
            HandleResult(err::result::ResultCouldNotBuildNSP, cfg::strings::Main.GetString(198));
            sd_exp->DeleteDirectory(consts::Root + "/dump");
            EnsureDirectories();
        }
        ncmContentStorageClose(&cnt_storage);
        ncmContentMetaDatabaseClose(&cnt_meta_db);
    }

}