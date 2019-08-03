#include <ui/ui_UpdateLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    UpdateLayout::UpdateLayout()
    {
        this->infoText = new pu::ui::elm::TextBlock(150, 320, "(...)");
        this->infoText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->downloadBar = new pu::ui::elm::ProgressBar(340, 360, 600, 30, 100.0f);
        this->Add(this->infoText);
        this->Add(this->downloadBar);
    }

    UpdateLayout::~UpdateLayout()
    {
        delete this->infoText;
        delete this->downloadBar;
    }

    void UpdateLayout::StartUpdateSearch()
    {
        if(!net::HasConnection())
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(304), { set::GetDictionaryEntry(234) }, true);
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->downloadBar->SetVisible(false);
        this->infoText->SetText(set::GetDictionaryEntry(305));
        mainapp->CallForRender();
        std::string js = net::RetrieveContent("https://api.github.com/repos/xortroll/goldleaf/releases", "application/json");
        JSON j = JSON::parse(js);
        std::string latestid = j[0]["tag_name"].get<std::string>();
        this->infoText->SetText(set::GetDictionaryEntry(306));
        mainapp->CallForRender();
        Version latestv = Version::FromString(latestid);
        Version currentv = Version::FromString(GOLDLEAF_VERSION); // Defined in Makefile
        if(latestv.IsEqual(currentv))
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(307), { set::GetDictionaryEntry(234) }, true);
        }
        else if(latestv.IsLower(currentv))
        {
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(308), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt == 0)
            {
                std::string baseurl = "https://github.com/XorTroll/Goldleaf/releases/download/" + latestid + "/Goldleaf";
                fs::CreateDirectory("sdmc:/switch/Goldleaf");
                fs::DeleteFile("sdmc:/switch/Goldleaf/Goldleaf.nro");
                this->infoText->SetText(set::GetDictionaryEntry(309));
                mainapp->CallForRender();
                this->downloadBar->SetVisible(true);
                net::RetrieveToFile(baseurl + ".nro", "sdmc:/switch/Goldleaf/Goldleaf.nro", [&](double Done, double Total)
                {
                    this->downloadBar->SetMaxValue(Total);
                    this->downloadBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->downloadBar->SetVisible(false);
                mainapp->CallForRender();
                sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(310), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                if(sopt == 0)
                {
                    std::string nspfile = "sdmc:/switch/Goldleaf/Goldleaf.nsp";
                    fs::DeleteFile(nspfile);
                    this->infoText->SetText(set::GetDictionaryEntry(311));
                    mainapp->CallForRender();
                    this->downloadBar->SetVisible(true);
                    net::RetrieveToFile(baseurl + ".nsp", nspfile, [&](double Done, double Total)
                    {
                        this->downloadBar->SetMaxValue(Total);
                        this->downloadBar->SetProgress(Done);
                        mainapp->CallForRender();
                    });
                    this->downloadBar->SetVisible(false);
                    this->infoText->SetText(set::GetDictionaryEntry(312));
                    mainapp->CallForRender();
                    if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, GOLDLEAF_APPID))
                    {
                        this->infoText->SetText(set::GetDictionaryEntry(313));
                        mainapp->CallForRender();
                        auto titles = hos::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
                        for(u32 i = 0; i < titles.size(); i++)
                        {
                            if(titles[i].ApplicationId == GOLDLEAF_APPID)
                            {
                                hos::RemoveTitle(titles[i]);
                                break;
                            }
                        }
                    }
                    else if(hos::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, GOLDLEAF_APPID))
                    {
                        this->infoText->SetText(set::GetDictionaryEntry(313));
                        mainapp->CallForRender();
                        auto titles = hos::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
                        for(u32 i = 0; i < titles.size(); i++)
                        {
                            if(titles[i].ApplicationId == GOLDLEAF_APPID)
                            {
                                hos::RemoveTitle(titles[i]);
                                break;
                            }
                        }
                    }
                    this->infoText->SetText(set::GetDictionaryEntry(312));
                    mainapp->CallForRender();
                    sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                    if(sopt < 0) return;
                    Storage dst = Storage::SdCard;
                    if(sopt == 0) dst = Storage::SdCard;
                    else if(sopt == 1) dst = Storage::NANDUser;
                    u64 fsize = fs::GetFileSize(nspfile);
                    u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                    if(rsize < fsize)
                    {
                        HandleResult(err::Make(err::ErrorDescription::NotEnoughSize), set::GetDictionaryEntry(251));
                        return;
                    }
                    mainapp->LoadLayout(mainapp->GetInstallLayout());
                    mainapp->GetInstallLayout()->StartInstall(nspfile, fs::GetSdCardExplorer(), dst, mainapp->GetMainMenuLayout());
                    mainapp->ShowNotification(set::GetDictionaryEntry(314) + " " + set::GetDictionaryEntry(315));
                }
                else mainapp->ShowNotification(set::GetDictionaryEntry(314) + " " + set::GetDictionaryEntry(315));
            }
        }
        else if(latestv.IsHigher(currentv)) mainapp->CreateShowDialog(set::GetDictionaryEntry(284), set::GetDictionaryEntry(316), { set::GetDictionaryEntry(234) }, true);
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }
}