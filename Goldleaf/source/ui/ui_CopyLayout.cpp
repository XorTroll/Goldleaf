
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    CopyLayout::CopyLayout() {
        this->copy_total_p_bar = pu::ui::elm::ProgressBar::New(0, 180, 600, 30, 100.0f);
        this->copy_total_p_bar->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        g_Settings.ApplyProgressBarColor(this->copy_total_p_bar);
        this->Add(this->copy_total_p_bar);

        this->total_info_text = pu::ui::elm::TextBlock::New(0, 220, cfg::Strings.GetString(469));
        this->total_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->total_info_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->total_info_text);

        this->copy_file_p_bar = pu::ui::elm::ProgressBar::New(0, 260, 600, 30, 100.0f);
        this->copy_file_p_bar->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        g_Settings.ApplyProgressBarColor(this->copy_file_p_bar);
        this->Add(this->copy_file_p_bar);

        this->file_info_text = pu::ui::elm::TextBlock::New(0, 300, cfg::Strings.GetString(470));
        this->file_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->file_info_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->file_info_text);

        this->copy_info_text = pu::ui::elm::TextBlock::New(0, 450, "A");
        this->copy_info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->copy_info_text->SetColor(g_Settings.custom_scheme.text);
        this->Add(this->copy_info_text);
    }

    void CopyLayout::StartCopy(const std::string &path, const std::string &new_path) {
        auto exp = fs::GetExplorerForPath(path);
        auto new_exp = fs::GetExplorerForPath(new_path);
        if(exp->IsDirectory(path)) {
            if(new_exp->IsDirectory(new_path)) {
                const auto option = g_MainApplication->CreateShowDialog(cfg::Strings.GetString(135), cfg::Strings.GetString(471), { cfg::Strings.GetString(111), cfg::Strings.GetString(112), cfg::Strings.GetString(18) }, true);
                if(option == 0) {
                    new_exp->DeleteDirectory(new_path);
                }
                else if(option != 1) {
                    return;
                }
            }

            const auto dir_name = fs::GetBaseName(path);
            hos::LockExit();
            g_MainApplication->LoadMenuHead(cfg::Strings.GetString(473));
            this->copy_file_p_bar->SetVisible(true);
            this->file_info_text->SetVisible(true);

            std::string cur_files_text;
            auto last_tp = std::chrono::steady_clock::now();
            exp->CopyDirectoryProgress(path, new_path, [&](const size_t total_size) {
                this->copy_total_p_bar->SetMaxProgress(total_size);
            }, [&](const size_t file_size, const std::string &old_file, const std::string &new_file) {
                cur_files_text = exp->MakePresentablePath(old_file) + "\n↓\n" + new_exp->MakePresentablePath(new_file);
                this->copy_file_p_bar->SetMaxProgress(file_size);
                this->copy_file_p_bar->SetProgress(0);
            }, [&](const size_t cur_rw_size) {
                this->copy_total_p_bar->IncrementProgress(cur_rw_size);
                this->copy_file_p_bar->IncrementProgress(cur_rw_size);

                const auto cur_tp = std::chrono::steady_clock::now();
                const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                last_tp = cur_tp;
                // By elapsed time and rw size, compute how much data has been written in 1 second
                const auto speed_bps = (1000.0f / time_diff) * (double)cur_rw_size;
                const auto speed_text = cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + hos::FormatTime((u64)((1.0f / speed_bps) * (this->copy_file_p_bar->GetMaxProgress() - this->copy_file_p_bar->GetProgress())));

                this->copy_info_text->SetText(cur_files_text + "\n\n" + speed_text);

                g_MainApplication->CallForRender();
            });
            hos::UnlockExit();
            g_MainApplication->ShowNotification(cfg::Strings.GetString(141));
        }
        else {
            if(new_exp->IsFile(new_path)) {
                const auto option = g_MainApplication->CreateShowDialog(cfg::Strings.GetString(153), cfg::Strings.GetString(143), { cfg::Strings.GetString(239), cfg::Strings.GetString(18) }, true);
                if(option != 0) {
                    return;
                }
                new_exp->DeleteFile(new_path);
            }

            const auto file_name = fs::GetBaseName(path);
            hos::LockExit();
            g_MainApplication->LoadMenuHead(cfg::Strings.GetString(472));
            this->copy_file_p_bar->SetVisible(false);
            this->file_info_text->SetVisible(false);
            const auto base_text = exp->MakePresentablePath(path) + "\n↓\n" + new_exp->MakePresentablePath(new_path);

            auto last_tp = std::chrono::steady_clock::now();
            exp->CopyFileProgress(path, new_path, [&](const size_t file_size) {
                this->copy_total_p_bar->SetMaxProgress(file_size);
            }, [&](const size_t cur_rw_size) {
                this->copy_total_p_bar->IncrementProgress(cur_rw_size);

                const auto cur_tp = std::chrono::steady_clock::now();
                const auto time_diff = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_tp - last_tp).count();
                last_tp = cur_tp;
                // By elapsed time and rw size, compute how much data has been written in 1 second
                const auto speed_bps = (1000.0f / time_diff) * (double)cur_rw_size;
                const auto speed_text =  cfg::Strings.GetString(458) + ": " + fs::FormatSize(speed_bps) + "/s, " + cfg::Strings.GetString(459) + ": " + hos::FormatTime((u64)((1.0f / speed_bps) * (this->copy_total_p_bar->GetMaxProgress() - this->copy_total_p_bar->GetProgress())));

                this->copy_info_text->SetText(base_text + "\n\n" + speed_text);

                g_MainApplication->CallForRender();
            });
            hos::UnlockExit();
            g_MainApplication->ShowNotification(cfg::Strings.GetString(240));
        }
    }

}