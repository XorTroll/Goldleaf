
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

#include <ui/ui_MemoryLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        inline constexpr double ComputeSpacePercentage(const size_t free_space, const size_t total_space) {
            return (100.0f * (double)(total_space - free_space)) / (double)total_space;
        }

        inline std::string MakeSizeInformation(const size_t free_space, const size_t total_space) {
            return fs::FormatSize(free_space) + " " + cfg::Strings.GetString(221) + " / " + fs::FormatSize(total_space);
        }

    }

    void MemoryLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    MemoryLayout::MemoryLayout() : pu::ui::Layout() {
        s32 cur_entry_y = 300;
        const s32 entry_x = 120;

        #define _CREATE_MEMORY_ENTRY(obj_text, text_string, obj_space_bar, obj_free_text) { \
            obj_text = pu::ui::elm::TextBlock::New(entry_x + 10, cur_entry_y, cfg::Strings.GetString(text_string)); \
            obj_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::MediumLarge)); \
            obj_text->SetColor(g_Settings.GetColorScheme().text); \
            obj_space_bar = pu::ui::elm::ProgressBar::New(entry_x, cur_entry_y + 45, pu::ui::render::ScreenWidth - (2 * entry_x), 30, 100.0f); \
            obj_space_bar->SetProgressColor(g_Settings.GetColorScheme().progress_bar); \
            obj_space_bar->SetBackgroundColor(g_Settings.GetColorScheme().progress_bar_bg); \
            obj_free_text = pu::ui::elm::TextBlock::New(entry_x + 10, cur_entry_y + 90, "..."); \
            obj_free_text->SetColor(g_Settings.GetColorScheme().text); \
            obj_free_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium)); \
            cur_entry_y += 150; \
        }

        _CREATE_MEMORY_ENTRY(this->sd_text, 19, this->sd_space_bar, this->sd_free_text);
        _CREATE_MEMORY_ENTRY(this->nand_text, 79, this->nand_space_bar, this->nand_free_text);
        _CREATE_MEMORY_ENTRY(this->nand_safe_text, 27, this->nand_safe_space_bar, this->nand_safe_free_text);
        _CREATE_MEMORY_ENTRY(this->nand_user_text, 28, this->nand_user_space_bar, this->nand_user_free_text);
        _CREATE_MEMORY_ENTRY(this->nand_system_text, 29, this->nand_system_space_bar, this->nand_system_free_text);
        
        this->Add(this->sd_text);
        this->Add(this->sd_space_bar);
        this->Add(this->sd_free_text);

        this->Add(this->nand_text);
        this->Add(this->nand_space_bar);
        this->Add(this->nand_free_text);

        this->Add(this->nand_safe_text);
        this->Add(this->nand_safe_space_bar);
        this->Add(this->nand_safe_free_text);

        this->Add(this->nand_user_text);
        this->Add(this->nand_user_space_bar);
        this->Add(this->nand_user_free_text);

        this->Add(this->nand_system_text);
        this->Add(this->nand_system_space_bar);
        this->Add(this->nand_system_free_text);

        this->SetOnInput(std::bind(&MemoryLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void MemoryLayout::UpdateElements() {
        const auto sd_free_space = fs::GetFreeSpaceForPartition(fs::Partition::SdCard);
        const auto sd_total_space = fs::GetTotalSpaceForPartition(fs::Partition::SdCard);
        const auto nand_safe_free_space = fs::GetFreeSpaceForPartition(fs::Partition::NANDSafe);
        const auto nand_safe_total_space = fs::GetTotalSpaceForPartition(fs::Partition::NANDSafe);
        const auto nand_user_free_space = fs::GetFreeSpaceForPartition(fs::Partition::NANDUser);
        const auto nand_user_total_space = fs::GetTotalSpaceForPartition(fs::Partition::NANDUser);
        const auto nand_system_free_space = fs::GetFreeSpaceForPartition(fs::Partition::NANDSystem);
        const auto nand_system_total_space = fs::GetTotalSpaceForPartition(fs::Partition::NANDSystem);
        const auto nand_free_space = nand_safe_free_space + nand_user_free_space + nand_system_free_space;
        const auto nand_total_space = nand_safe_total_space + nand_user_total_space + nand_system_total_space;
        
        const auto sd_space_percentage = ComputeSpacePercentage(sd_free_space, sd_total_space);
        const auto nand_space_percentage = ComputeSpacePercentage(nand_free_space, nand_total_space);
        const auto nand_safe_space_percentage = ComputeSpacePercentage(nand_safe_free_space, nand_safe_total_space);
        const auto nand_user_space_percentage = ComputeSpacePercentage(nand_user_free_space, nand_user_total_space);
        const auto nand_system_space_percentage = ComputeSpacePercentage(nand_system_free_space, nand_system_total_space);

        this->sd_space_bar->SetProgress(sd_space_percentage);
        this->sd_free_text->SetText(MakeSizeInformation(sd_free_space, sd_total_space));

        this->nand_space_bar->SetProgress(nand_space_percentage);
        this->nand_free_text->SetText(MakeSizeInformation(nand_free_space, nand_total_space));

        this->nand_safe_space_bar->SetProgress(nand_safe_space_percentage);
        this->nand_safe_free_text->SetText(MakeSizeInformation(nand_safe_free_space, nand_safe_total_space));

        this->nand_user_space_bar->SetProgress(nand_user_space_percentage);
        this->nand_user_free_text->SetText(MakeSizeInformation(nand_user_free_space, nand_user_total_space));

        this->nand_system_space_bar->SetProgress(nand_system_space_percentage);
        this->nand_system_free_text->SetText(MakeSizeInformation(nand_system_free_space, nand_system_total_space));
    }

}
