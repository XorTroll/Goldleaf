
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

#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    CopyLayout::CopyLayout() {
        this->info_text = pu::ui::elm::TextBlock::New(0, 360, cfg::strings::Main.GetString(151));
        this->info_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->info_text->SetColor(g_Settings.custom_scheme.text);
        this->copy_total_p_bar = pu::ui::elm::ProgressBar::New(340, 320, 600, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->copy_total_p_bar);
        this->copy_file_p_bar = pu::ui::elm::ProgressBar::New(340, 400, 600, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->copy_file_p_bar);
        this->Add(this->info_text);
        this->Add(this->copy_total_p_bar);
        this->Add(this->copy_file_p_bar);
    }

    void CopyLayout::StartCopy(const std::string &path, const std::string &new_path) {
        auto exp = fs::GetExplorerForPath(path);
        auto new_exp = fs::GetExplorerForPath(new_path);
        if(exp->IsDirectory(path)) {
            if(new_exp->IsDirectory(new_path)) {
                const auto option = g_MainApplication->CreateShowDialog("Directory already exists", "...", { "Delete dir and continue", "Continue no delete", "Cancel" }, true);
                if(option == 0) {
                    new_exp->DeleteDirectory(new_path);
                }
                else if(option != 1) {
                    return;
                }
            }

            const auto dir_name = fs::GetFileName(path);
            hos::LockAutoSleep();
            this->copy_file_p_bar->SetVisible(true);
            exp->CopyDirectoryProgress(path, new_path, [&](const size_t cur_size, const size_t total_size, const std::string &cur_file_name, const size_t cur_file_size, const size_t total_file_size) {
                this->info_text->SetText(cur_file_name);
                this->copy_total_p_bar->SetMaxProgress(total_size);
                this->copy_total_p_bar->SetProgress(cur_size);
                this->copy_file_p_bar->SetMaxProgress(total_file_size);
                this->copy_file_p_bar->SetProgress(cur_file_size);
                g_MainApplication->CallForRender();
            });
            hos::UnlockAutoSleep();
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(141));
        }
        else {
            if(new_exp->IsFile(new_path)) {
                const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(153), cfg::strings::Main.GetString(143), { cfg::strings::Main.GetString(239), cfg::strings::Main.GetString(18) }, true);
                if(option != 0) {
                    return;
                }
            }
            new_exp->DeleteFile(new_path);

            const auto file_name = fs::GetFileName(path);
            hos::LockAutoSleep();
            this->copy_file_p_bar->SetVisible(false);
            this->info_text->SetText(file_name);
            exp->CopyFileProgress(path, new_path, [&](const size_t cur_size, const size_t total_size) {
                this->copy_total_p_bar->SetMaxProgress(total_size);
                this->copy_total_p_bar->SetProgress(cur_size);
                g_MainApplication->CallForRender();
            });
            hos::UnlockAutoSleep();
            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(240));
        }
    }

}