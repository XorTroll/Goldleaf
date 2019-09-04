#include <ui/ui_CopyLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    CopyLayout::CopyLayout()
    {
        this->infoText = pu::ui::elm::TextBlock::New(150, 320, set::GetDictionaryEntry(151));
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->copyBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        gsets.ApplyProgressBarColor(this->copyBar);
        this->Add(this->infoText);
        this->Add(this->copyBar);
    }

    void CopyLayout::StartCopy(pu::String Path, pu::String NewPath, bool Directory, fs::Explorer *Exp)
    {
        if(Directory)
        {
            fs::CopyDirectoryProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(141));
        }
        else
        {
            if(Exp->IsFile(NewPath))
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(153), set::GetDictionaryEntry(143), { set::GetDictionaryEntry(239), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
            }
            fs::DeleteFile(NewPath);
            fs::CopyFileProgress(Path, NewPath, [&](double done, double total)
            {
                this->copyBar->SetMaxValue(total);
                this->copyBar->SetProgress(done);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(240));
        }
    }
}