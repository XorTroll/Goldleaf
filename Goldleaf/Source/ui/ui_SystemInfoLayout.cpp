#include <ui/ui_SystemInfoLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    SystemInfoLayout::SystemInfoLayout() : pu::ui::Layout()
    {
        hos::FwVersion fwv = hos::GetFwVersion();
        this->fwText = new pu::ui::elm::TextBlock(40, 650, "Firmware: " + fwv.ToString() + " (" + fwv.DisplayName + ")");
        this->fwText->SetColor(gsets.CustomScheme.Text);
        this->sdText = new pu::ui::elm::TextBlock(125, 300, set::GetDictionaryEntry(19), 35);
        this->sdText->SetColor(gsets.CustomScheme.Text);
        this->sdBar = new pu::ui::elm::ProgressBar(120, 345, 450, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->sdBar);
        this->sdFreeText = new pu::ui::elm::TextBlock(125, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->sdFreeText->SetColor(gsets.CustomScheme.Text);
        this->nandText = new pu::ui::elm::TextBlock(615, 300, set::GetDictionaryEntry(79), 35);
        this->nandText->SetColor(gsets.CustomScheme.Text);
        this->nandBar = new pu::ui::elm::ProgressBar(620, 345, 450, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->nandBar);
        this->nandFreeText = new pu::ui::elm::TextBlock(620, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->nandFreeText->SetColor(gsets.CustomScheme.Text);
        this->safeText = new pu::ui::elm::TextBlock(105, 480, set::GetDictionaryEntry(27));
        this->safeText->SetColor(gsets.CustomScheme.Text);
        this->safeBar = new pu::ui::elm::ProgressBar(100, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->safeBar);
        this->userText = new pu::ui::elm::TextBlock(455, 480, set::GetDictionaryEntry(28));
        this->userText->SetColor(gsets.CustomScheme.Text);
        this->userBar = new pu::ui::elm::ProgressBar(450, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->userBar);
        this->systemText = new pu::ui::elm::TextBlock(805, 480, set::GetDictionaryEntry(29));
        this->systemText->SetColor(gsets.CustomScheme.Text);
        this->systemBar = new pu::ui::elm::ProgressBar(800, 515, 300, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->systemBar);
        this->Add(this->fwText);
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

    SystemInfoLayout::~SystemInfoLayout()
    {
        
    }

    void SystemInfoLayout::UpdateElements()
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