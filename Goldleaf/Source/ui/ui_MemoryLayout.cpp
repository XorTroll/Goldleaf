
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    MemoryLayout::MemoryLayout() : pu::ui::Layout()
    {
        this->sdText = pu::ui::elm::TextBlock::New(125, 300, cfg::strings::Main.GetString(19));
        this->sdText->SetFont("DefaultFont@35");
        this->sdText->SetColor(global_settings.custom_scheme.Text);
        this->sdBar = pu::ui::elm::ProgressBar::New(120, 345, 450, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->sdBar);
        this->sdFreeText = pu::ui::elm::TextBlock::New(125, 385, "0 bytes " + cfg::strings::Main.GetString(221));
        this->sdFreeText->SetColor(global_settings.custom_scheme.Text);
        this->nandText = pu::ui::elm::TextBlock::New(615, 300, cfg::strings::Main.GetString(79));
        this->nandText->SetFont("DefaultFont@35");
        this->nandText->SetColor(global_settings.custom_scheme.Text);
        this->nandBar = pu::ui::elm::ProgressBar::New(620, 345, 450, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->nandBar);
        this->nandFreeText = pu::ui::elm::TextBlock::New(620, 385, "0 bytes " + cfg::strings::Main.GetString(221));
        this->nandFreeText->SetColor(global_settings.custom_scheme.Text);
        this->safeText = pu::ui::elm::TextBlock::New(105, 480, cfg::strings::Main.GetString(27));
        this->safeText->SetColor(global_settings.custom_scheme.Text);
        this->safeBar = pu::ui::elm::ProgressBar::New(100, 515, 300, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->safeBar);
        this->userText = pu::ui::elm::TextBlock::New(455, 480, cfg::strings::Main.GetString(28));
        this->userText->SetColor(global_settings.custom_scheme.Text);
        this->userBar = pu::ui::elm::ProgressBar::New(450, 515, 300, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->userBar);
        this->systemText = pu::ui::elm::TextBlock::New(805, 480, cfg::strings::Main.GetString(29));
        this->systemText->SetColor(global_settings.custom_scheme.Text);
        this->systemBar = pu::ui::elm::ProgressBar::New(800, 515, 300, 30, 100.0f);
        global_settings.ApplyProgressBarColor(this->systemBar);
        this->Add(this->sdText);
        this->Add(this->sdBar);
        this->Add(this->sdFreeText);
        this->Add(this->nandText);
        this->Add(this->nandBar);
        this->Add(this->nandFreeText);
        this->Add(this->safeText);
        this->Add(this->safeBar);
        this->Add(this->userText);
        this->Add(this->userBar);
        this->Add(this->systemText);
        this->Add(this->systemBar);
    }

    void MemoryLayout::UpdateElements()
    {
        u64 sdctotal = fs::GetTotalSpaceForPartition(fs::Partition::SdCard);
        u64 sdcfree = fs::GetFreeSpaceForPartition(fs::Partition::SdCard);
        u64 nsftotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDSafe);
        u64 nsffree = fs::GetFreeSpaceForPartition(fs::Partition::NANDSafe);
        u64 nsutotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDUser);
        u64 nsufree = fs::GetFreeSpaceForPartition(fs::Partition::NANDUser);
        u64 nsstotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDSystem);
        u64 nssfree = fs::GetFreeSpaceForPartition(fs::Partition::NANDSystem);
        u8 sdval = ((100 * (sdctotal - sdcfree)) / sdctotal);
        u8 nsfval = ((100 * (nsftotal - nsffree)) / nsftotal);
        u8 nsuval = ((100 * (nsutotal - nsufree)) / nsutotal);
        u8 nssval = ((100 * (nsstotal - nssfree)) / nsstotal);
        this->sdBar->SetProgress(sdval);
        this->safeBar->SetProgress(nsfval);
        this->userBar->SetProgress(nsuval);
        this->systemBar->SetProgress(nssval);
        u64 nandtotal = (nsftotal + nsutotal + nsstotal);
        u64 nandfree = (nsffree + nsufree + nssfree);
        u8 nandval = ((100 * (nandtotal - nandfree)) / nandtotal);
        this->nandBar->SetProgress(nandval);
        this->nandFreeText->SetText(fs::FormatSize(nandfree) + " " + cfg::strings::Main.GetString(221));
        this->sdFreeText->SetText(fs::FormatSize(sdcfree) + " " + cfg::strings::Main.GetString(221));
    }
}