
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    MemoryLayout::MemoryLayout() : pu::ui::Layout()
    {
        this->sdText = pu::ui::elm::TextBlock::New(125, 300, set::GetDictionaryEntry(19), 35);
        this->sdText->SetColor(gsets.CustomScheme.Text);
        this->sdBar = pu::ui::elm::ProgressBar::New(120, 345, 450, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->sdBar);
        this->sdFreeText = pu::ui::elm::TextBlock::New(125, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->sdFreeText->SetColor(gsets.CustomScheme.Text);
        this->nandText = pu::ui::elm::TextBlock::New(615, 300, set::GetDictionaryEntry(79), 35);
        this->nandText->SetColor(gsets.CustomScheme.Text);
        this->nandBar = pu::ui::elm::ProgressBar::New(620, 345, 450, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->nandBar);
        this->nandFreeText = pu::ui::elm::TextBlock::New(620, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->nandFreeText->SetColor(gsets.CustomScheme.Text);
        this->safeText = pu::ui::elm::TextBlock::New(105, 480, set::GetDictionaryEntry(27));
        this->safeText->SetColor(gsets.CustomScheme.Text);
        this->safeBar = pu::ui::elm::ProgressBar::New(100, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->safeBar);
        this->userText = pu::ui::elm::TextBlock::New(455, 480, set::GetDictionaryEntry(28));
        this->userText->SetColor(gsets.CustomScheme.Text);
        this->userBar = pu::ui::elm::ProgressBar::New(450, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->userBar);
        this->systemText = pu::ui::elm::TextBlock::New(805, 480, set::GetDictionaryEntry(29));
        this->systemText->SetColor(gsets.CustomScheme.Text);
        this->systemBar = pu::ui::elm::ProgressBar::New(800, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->systemBar);
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
        this->nandFreeText->SetText(fs::FormatSize(nandfree) + " " + set::GetDictionaryEntry(221));
        this->sdFreeText->SetText(fs::FormatSize(sdcfree) + " " + set::GetDictionaryEntry(221));
    }
}