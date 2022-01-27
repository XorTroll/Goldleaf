
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
            return fs::FormatSize(free_space) + " " + cfg::strings::Main.GetString(221) + " / " + fs::FormatSize(total_space);
        }

    }

    MemoryLayout::MemoryLayout() : pu::ui::Layout() {
        this->sd_text = pu::ui::elm::TextBlock::New(125, 300, cfg::strings::Main.GetString(19));
        this->sd_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Large));
        this->sd_text->SetColor(g_Settings.custom_scheme.text);
        this->sd_space_bar = pu::ui::elm::ProgressBar::New(120, 345, 450, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->sd_space_bar);
        this->sd_free_text = pu::ui::elm::TextBlock::New(125, 385, "...");
        this->sd_free_text->SetColor(g_Settings.custom_scheme.text);
    
        this->nand_text = pu::ui::elm::TextBlock::New(615, 300, cfg::strings::Main.GetString(79));
        this->nand_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Large));
        this->nand_text->SetColor(g_Settings.custom_scheme.text);
        this->nand_space_bar = pu::ui::elm::ProgressBar::New(620, 345, 450, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->nand_space_bar);
        this->nand_free_text = pu::ui::elm::TextBlock::New(620, 385, "...");
        this->nand_free_text->SetColor(g_Settings.custom_scheme.text);
    
        this->nand_safe_text = pu::ui::elm::TextBlock::New(105, 480, cfg::strings::Main.GetString(27));
        this->nand_safe_text->SetColor(g_Settings.custom_scheme.text);
        this->nand_safe_space_bar = pu::ui::elm::ProgressBar::New(100, 515, 300, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->nand_safe_space_bar);
        this->nand_safe_free_text = pu::ui::elm::TextBlock::New(105, 555, "...");
        this->nand_safe_free_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->nand_safe_free_text->SetColor(g_Settings.custom_scheme.text);
        
        this->nand_user_text = pu::ui::elm::TextBlock::New(455, 480, cfg::strings::Main.GetString(28));
        this->nand_user_text->SetColor(g_Settings.custom_scheme.text);
        this->nand_user_space_bar = pu::ui::elm::ProgressBar::New(450, 515, 300, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->nand_user_space_bar);
        this->nand_user_free_text = pu::ui::elm::TextBlock::New(455, 555, "...");
        this->nand_user_free_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->nand_user_free_text->SetColor(g_Settings.custom_scheme.text);
        
        this->nand_system_text = pu::ui::elm::TextBlock::New(805, 480, cfg::strings::Main.GetString(29));
        this->nand_system_text->SetColor(g_Settings.custom_scheme.text);
        this->nand_system_space_bar = pu::ui::elm::ProgressBar::New(800, 515, 300, 30, 100.0f);
        g_Settings.ApplyProgressBarColor(this->nand_system_space_bar);
        this->nand_system_free_text = pu::ui::elm::TextBlock::New(805, 555, "...");
        this->nand_system_free_text->SetFont(pu::ui::GetDefaultFont(pu::ui::DefaultFontSize::Medium));
        this->nand_system_free_text->SetColor(g_Settings.custom_scheme.text);
        
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