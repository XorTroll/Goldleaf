#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    CopyLayout::CopyLayout()
    {
        this->infoText = new pu::element::TextBlock(150, 320, set::GetDictionaryEntry(151));
        this->infoText->SetHorizontalAlign(pu::element::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->copyBar = new pu::element::ProgressBar(340, 360, 600, 30, 100.0f);
        this->Add(this->infoText);
        this->Add(this->copyBar);
    }

    CopyLayout::~CopyLayout()
    {
        delete this->infoText;
        delete this->copyBar;
    }

    void CopyLayout::StartCopy(std::string Path, std::string NewPath, bool Directory, fs::Explorer *Exp, pu::Layout *Prev)
    {
        if(Directory)
        {
            fs::CopyDirectoryProgress(Path, NewPath, [&](u8 p)
            {
                this->copyBar->SetProgress(p);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(141));
        }
        else
        {
            if(Exp->IsFile(NewPath))
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(142), set::GetDictionaryEntry(143), { set::GetDictionaryEntry(239), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0)
                {
                    mainapp->LoadLayout(Prev);
                    return;
                }
            }
            fs::CopyFileProgress(Path, NewPath, [&](u8 p)
            {
                this->copyBar->SetProgress(p);
                mainapp->CallForRender();
            });
            mainapp->ShowNotification(set::GetDictionaryEntry(240));
        }
        mainapp->LoadLayout(Prev);
    }
}