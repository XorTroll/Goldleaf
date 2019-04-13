#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    UpdateLayout::UpdateLayout()
    {
        this->infoText = new pu::element::TextBlock(150, 320, "Utest");
        this->infoText->SetHorizontalAlign(pu::element::HorizontalAlign::Center);
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->downloadBar = new pu::element::ProgressBar(340, 360, 600, 30, 100.0f);
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
            mainapp->CreateShowDialog("Connection", "No internet connection was found.", { "Ok" }, true);
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->downloadBar->SetVisible(false);
        this->infoText->SetText("Accessing GitHub metadata for releases' information...");
        mainapp->CallForRender();
        std::string js = net::RetrieveContent("https://api.github.com/repos/xortroll/goldleaf/releases", "application/json");
        json j = json::parse(js);
        std::string latestid = j[0]["tag_name"].get<std::string>();
        this->infoText->SetText("Access completed (latest release version: v" + latestid);
        mainapp->CallForRender();
        Version latestv = Version::FromString(latestid);
        Version currentv = Version::FromString("0.3");
        if(latestv.IsEqual(currentv))
        {
            mainapp->CreateShowDialog("Update search", "Goldleaf's version matches latest release's one.", { "Ok" }, true);
        }
        else if(latestv.IsLower(currentv))
        {
            int sopt = mainapp->CreateShowDialog("Update search (hardcoded low value for testing)", "Latest release is v" + latestid + "\nWould you like to update Goldleaf to this version?\n(don't select it, testing only)", { "Yes", "Cancel" }, true);
            if(sopt == 0)
            {
                std::string baseurl = "https://github.com/XorTroll/Goldleaf/releases/download/" + latestid + "/Goldleaf";
                fs::CreateDirectory("sdmc:/switch/Goldleaf");
                fs::DeleteFile("sdmc:/switch/Goldleaf/Goldleaf.nro");
                this->infoText->SetText("Downloading latest release NRO...");
                mainapp->CallForRender();
                this->downloadBar->SetVisible(true);
                net::RetrieveToFile(baseurl + ".nro", "sdmc:/switch/Goldleaf/Goldleaf.nro", [&](double Done, double Total)
                {
                    this->downloadBar->SetMaxValue(Total);
                    this->downloadBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->downloadBar->SetVisible(false);
                this->infoText->SetText("NRO downloaded fine.");
                mainapp->CallForRender();
                sopt = mainapp->CreateShowDialog("Update search", "Would you like to download and install the NSP too?", { "Yes", "Cancel" }, true);
                if(sopt == 0)
                {
                    std::string nspfile = "sdmc:/switch/Goldleaf/Goldleaf.nsp";
                    fs::DeleteFile(nspfile);
                    this->infoText->SetText("Downloading latest release NSP...");
                    mainapp->CallForRender();
                    this->downloadBar->SetVisible(true);
                    net::RetrieveToFile(baseurl + ".nsp", nspfile, [&](double Done, double Total)
                    {
                        this->downloadBar->SetMaxValue(Total);
                        this->downloadBar->SetProgress(Done);
                        mainapp->CallForRender();
                    });
                    this->downloadBar->SetVisible(false);
                    this->infoText->SetText("NSP downloaded fine. Preparing to install it...");
                    mainapp->CallForRender();
                    if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, GOLDLEAF_APPID))
                    {
                        this->infoText->SetText("Removing old installation...");
                        mainapp->CallForRender();
                        auto titles = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
                        for(u32 i = 0; i < titles.size(); i++)
                        {
                            if(titles[i].ApplicationId == GOLDLEAF_APPID)
                            {
                                horizon::RemoveTitle(titles[i]);
                                break;
                            }
                        }
                    }
                    else if(horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, GOLDLEAF_APPID))
                    {
                        this->infoText->SetText("Removing old installation...");
                        mainapp->CallForRender();
                        auto titles = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
                        for(u32 i = 0; i < titles.size(); i++)
                        {
                            if(titles[i].ApplicationId == GOLDLEAF_APPID)
                            {
                                horizon::RemoveTitle(titles[i]);
                                break;
                            }
                        }
                    }
                    this->infoText->SetText("Starting installation...");
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
                    mainapp->ShowNotification("Goldleaf has been updated and reinstalled. Please restart it to use the new one.");
                }
                else mainapp->ShowNotification("Goldleaf has been updated. Please restart it to use the new one.");
            }
        }
        else if(latestv.IsHigher(currentv))
        {
            mainapp->CreateShowDialog("Update search", "Weird version mismatch (Goldleaf's version is higher than the latest release's one)\nAre you sure this Goldleaf is the official one?", { "Ok" }, true);
        }
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }
}