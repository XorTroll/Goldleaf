#include <gleaf/ui/MainApplication.hpp>
#include <gleaf/ui/Utils.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    MainApplication *mainapp;
    extern std::string clipboard;

    MainApplication::MainApplication() : pu::Application()
    {
        gsets = set::ProcessSettings();
        set::Initialize();
        this->SetBackgroundColor(gsets.CustomScheme.Background);
        this->preblv = 0;
        this->preisch = false;
        this->pretime = "";
        this->vfirst = true;
        this->connstate = 0;
        this->baseImage = new pu::element::Image(0, 0, gsets.PathForResource("/Base.png"));
        this->timeText = new pu::element::TextBlock(1124, 20, "00:00:00");
        this->timeText->SetColor(gsets.CustomScheme.Text);
        this->batteryText = new pu::element::TextBlock(1015, 22, "0%", 20);
        this->batteryText->SetColor(gsets.CustomScheme.Text);
        this->batteryImage = new pu::element::Image(960, 8, gsets.PathForResource("/Battery/0.png"));
        this->batteryChargeImage = new pu::element::Image(960, 8, gsets.PathForResource("/Battery/Charge.png"));
        this->menuBanner = new pu::element::Image(10, 62, gsets.PathForResource("/MenuBanner.png"));
        this->menuImage = new pu::element::Image(10, 67, gsets.PathForResource("/Common/SdCard.png"));
        this->menuImage->SetWidth(100);
        this->menuImage->SetHeight(100);
        this->usbImage = new pu::element::Image(890, 12, gsets.PathForResource("/Common/USB.png"));
        this->usbImage->SetWidth(40);
        this->usbImage->SetHeight(40);
        this->usbImage->SetVisible(false);
        this->connImage = new pu::element::Image(830, 12, gsets.PathForResource("/Connection/None.png"));
        this->connImage->SetWidth(40);
        this->connImage->SetHeight(40);
        this->connImage->SetVisible(true);
        this->menuNameText = new pu::element::TextBlock(120, 90, "-");
        this->menuNameText->SetColor(gsets.CustomScheme.Text);
        this->menuHeadText = new pu::element::TextBlock(120, 125, "-", 20);
        this->menuHeadText->SetColor(gsets.CustomScheme.Text);
        this->UnloadMenuData();
        this->footerText = new pu::element::TextBlock(15, 685, set::GetDictionaryEntry(9), 20);
        this->footerText->SetColor(gsets.CustomScheme.Text);
        this->UpdateValues();
        this->mainMenu = new MainMenuLayout();
        this->browser = new PartitionBrowserLayout();
        this->browser->SetOnInput(std::bind(&MainApplication::browser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->fileContent = new FileContentLayout();
        this->fileContent->SetOnInput(std::bind(&MainApplication::fileContent_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->copy = new CopyLayout();
        this->nspInstall = new InstallLayout();
        this->usbInstall = new USBInstallLayout();
        this->usbInstall->SetOnInput(std::bind(&MainApplication::usbInstall_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->contentInformation = new ContentInformationLayout();
        this->contentInformation->SetOnInput(std::bind(&MainApplication::contentInformation_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->storageContents = new StorageContentsLayout();
        this->storageContents->SetOnInput(std::bind(&MainApplication::storageContents_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->contentManager = new ContentManagerLayout();
        this->contentManager->SetOnInput(std::bind(&MainApplication::contentManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->titleDump = new TitleDumperLayout();
        this->ticketManager = new TicketManagerLayout();
        this->ticketManager->SetOnInput(std::bind(&MainApplication::ticketManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->account = new AccountLayout();
        this->account->SetOnInput(std::bind(&MainApplication::account_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sysInfo = new SystemInfoLayout();
        this->sysInfo->SetOnInput(std::bind(&MainApplication::sysInfo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->update = new UpdateLayout();
        this->about = new AboutLayout();
        this->about->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->mainMenu->AddChild(this->baseImage);
        this->browser->AddChild(this->baseImage);
        this->fileContent->AddChild(this->baseImage);
        this->copy->AddChild(this->baseImage);
        this->nspInstall->AddChild(this->baseImage);
        this->usbInstall->AddChild(this->baseImage);
        this->contentInformation->AddChild(this->baseImage);
        this->storageContents->AddChild(this->baseImage);
        this->contentManager->AddChild(this->baseImage);
        this->titleDump->AddChild(this->baseImage);
        this->ticketManager->AddChild(this->baseImage);
        this->account->AddChild(this->baseImage);
        this->sysInfo->AddChild(this->baseImage);
        this->update->AddChild(this->baseImage);
        this->about->AddChild(this->baseImage);
        this->mainMenu->AddChild(this->timeText);
        this->browser->AddChild(this->timeText);
        this->fileContent->AddChild(this->timeText);
        this->copy->AddChild(this->timeText);
        this->nspInstall->AddChild(this->timeText);
        this->usbInstall->AddChild(this->timeText);
        this->contentInformation->AddChild(this->timeText);
        this->storageContents->AddChild(this->timeText);
        this->contentManager->AddChild(this->timeText);
        this->titleDump->AddChild(this->timeText);
        this->ticketManager->AddChild(this->timeText);
        this->account->AddChild(this->timeText);
        this->sysInfo->AddChild(this->timeText);
        this->update->AddChild(this->timeText);
        this->about->AddChild(this->timeText);
        this->mainMenu->AddChild(this->batteryText);
        this->browser->AddChild(this->batteryText);
        this->fileContent->AddChild(this->batteryText);
        this->copy->AddChild(this->batteryText);
        this->nspInstall->AddChild(this->batteryText);
        this->usbInstall->AddChild(this->batteryText);
        this->contentInformation->AddChild(this->batteryText);
        this->storageContents->AddChild(this->batteryText);
        this->contentManager->AddChild(this->batteryText);
        this->titleDump->AddChild(this->batteryText);
        this->ticketManager->AddChild(this->batteryText);
        this->account->AddChild(this->batteryText);
        this->sysInfo->AddChild(this->batteryText);
        this->update->AddChild(this->batteryText);
        this->about->AddChild(this->batteryText);
        this->mainMenu->AddChild(this->batteryImage);
        this->browser->AddChild(this->batteryImage);
        this->fileContent->AddChild(this->batteryImage);
        this->copy->AddChild(this->batteryImage);
        this->nspInstall->AddChild(this->batteryImage);
        this->usbInstall->AddChild(this->batteryImage);
        this->contentInformation->AddChild(this->batteryImage);
        this->storageContents->AddChild(this->batteryImage);
        this->contentManager->AddChild(this->batteryImage);
        this->titleDump->AddChild(this->batteryImage);
        this->ticketManager->AddChild(this->batteryImage);
        this->account->AddChild(this->batteryImage);
        this->sysInfo->AddChild(this->batteryImage);
        this->update->AddChild(this->batteryImage);
        this->about->AddChild(this->batteryImage);
        this->mainMenu->AddChild(this->batteryChargeImage);
        this->browser->AddChild(this->batteryChargeImage);
        this->fileContent->AddChild(this->batteryChargeImage);
        this->copy->AddChild(this->batteryChargeImage);
        this->nspInstall->AddChild(this->batteryChargeImage);
        this->usbInstall->AddChild(this->batteryChargeImage);
        this->contentInformation->AddChild(this->batteryChargeImage);
        this->storageContents->AddChild(this->batteryChargeImage);
        this->contentManager->AddChild(this->batteryChargeImage);
        this->titleDump->AddChild(this->batteryChargeImage);
        this->ticketManager->AddChild(this->batteryChargeImage);
        this->account->AddChild(this->batteryChargeImage);
        this->sysInfo->AddChild(this->batteryChargeImage);
        this->update->AddChild(this->batteryChargeImage);
        this->about->AddChild(this->batteryChargeImage);
        this->mainMenu->AddChild(this->menuImage);
        this->browser->AddChild(this->menuImage);
        this->fileContent->AddChild(this->menuImage);
        this->copy->AddChild(this->menuImage);
        this->nspInstall->AddChild(this->menuImage);
        this->usbInstall->AddChild(this->menuImage);
        this->contentInformation->AddChild(this->menuImage);
        this->storageContents->AddChild(this->menuImage);
        this->contentManager->AddChild(this->menuImage);
        this->titleDump->AddChild(this->menuImage);
        this->ticketManager->AddChild(this->menuImage);
        this->account->AddChild(this->menuImage);
        this->sysInfo->AddChild(this->menuImage);
        this->update->AddChild(this->menuImage);
        this->about->AddChild(this->menuImage);
        this->mainMenu->AddChild(this->usbImage);
        this->browser->AddChild(this->usbImage);
        this->fileContent->AddChild(this->usbImage);
        this->copy->AddChild(this->usbImage);
        this->nspInstall->AddChild(this->usbImage);
        this->usbInstall->AddChild(this->usbImage);
        this->contentInformation->AddChild(this->usbImage);
        this->storageContents->AddChild(this->usbImage);
        this->contentManager->AddChild(this->usbImage);
        this->titleDump->AddChild(this->usbImage);
        this->ticketManager->AddChild(this->usbImage);
        this->account->AddChild(this->usbImage);
        this->sysInfo->AddChild(this->usbImage);
        this->update->AddChild(this->usbImage);
        this->about->AddChild(this->usbImage);
        this->mainMenu->AddChild(this->connImage);
        this->browser->AddChild(this->connImage);
        this->fileContent->AddChild(this->connImage);
        this->copy->AddChild(this->connImage);
        this->nspInstall->AddChild(this->connImage);
        this->usbInstall->AddChild(this->connImage);
        this->contentInformation->AddChild(this->connImage);
        this->storageContents->AddChild(this->connImage);
        this->contentManager->AddChild(this->connImage);
        this->titleDump->AddChild(this->connImage);
        this->ticketManager->AddChild(this->connImage);
        this->account->AddChild(this->connImage);
        this->sysInfo->AddChild(this->connImage);
        this->update->AddChild(this->connImage);
        this->about->AddChild(this->connImage);
        this->mainMenu->AddChild(this->menuBanner);
        this->mainMenu->AddChild(this->menuNameText);
        this->browser->AddChild(this->menuNameText);
        this->fileContent->AddChild(this->menuNameText);
        this->copy->AddChild(this->menuNameText);
        this->nspInstall->AddChild(this->menuNameText);
        this->usbInstall->AddChild(this->menuNameText);
        this->contentInformation->AddChild(this->menuNameText);
        this->storageContents->AddChild(this->menuNameText);
        this->contentManager->AddChild(this->menuNameText);
        this->titleDump->AddChild(this->menuNameText);
        this->ticketManager->AddChild(this->menuNameText);
        this->account->AddChild(this->menuNameText);
        this->sysInfo->AddChild(this->menuNameText);
        this->update->AddChild(this->menuNameText);
        this->about->AddChild(this->menuNameText);
        this->mainMenu->AddChild(this->menuHeadText);
        this->browser->AddChild(this->menuHeadText);
        this->fileContent->AddChild(this->menuHeadText);
        this->copy->AddChild(this->menuHeadText);
        this->nspInstall->AddChild(this->menuHeadText);
        this->usbInstall->AddChild(this->menuHeadText);
        this->contentInformation->AddChild(this->menuHeadText);
        this->storageContents->AddChild(this->menuHeadText);
        this->contentManager->AddChild(this->menuHeadText);
        this->titleDump->AddChild(this->menuHeadText);
        this->ticketManager->AddChild(this->menuHeadText);
        this->account->AddChild(this->menuHeadText);
        this->sysInfo->AddChild(this->menuHeadText);
        this->update->AddChild(this->menuHeadText);
        this->about->AddChild(this->menuHeadText);
        this->mainMenu->AddChild(this->footerText);
        this->browser->AddChild(this->footerText);
        this->fileContent->AddChild(this->footerText);
        this->copy->AddChild(this->footerText);
        this->nspInstall->AddChild(this->footerText);
        this->usbInstall->AddChild(this->footerText);
        this->contentInformation->AddChild(this->footerText);
        this->storageContents->AddChild(this->footerText);
        this->contentManager->AddChild(this->footerText);
        this->titleDump->AddChild(this->footerText);
        this->ticketManager->AddChild(this->footerText);
        this->account->AddChild(this->footerText);
        this->sysInfo->AddChild(this->footerText);
        this->update->AddChild(this->footerText);
        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->LoadLayout(this->mainMenu);
    }

    void MainApplication::UpdateFooter(std::string Text)
    {
        this->footerText->SetText(Text);
    }

    void MainApplication::UpdateValues()
    {
        /*
        if(this->vfirst)
        {
            if(net::CheckVersionDiff()) mainapp->CreateShowDialog("Outdated Goldleaf", "New updates were found.\nGo to the updates section to update Goldleaf.", { "Ok" }, true);
        }*/
        std::string dtime = horizon::GetCurrentTime();
        u32 blv = horizon::GetBatteryLevel();
        bool isch = horizon::IsCharging();
        if((this->preblv != blv) || this->vfirst)
        {
            if(blv <= 10) this->batteryImage->SetImage(gsets.PathForResource("/Battery/0.png"));
            else if((blv > 10) && (blv <= 20)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/10.png"));
            else if((blv > 20) && (blv <= 30)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/20.png"));
            else if((blv > 30) && (blv <= 40)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/30.png"));
            else if((blv > 40) && (blv <= 50)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/40.png"));
            else if((blv > 50) && (blv <= 60)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/50.png"));
            else if((blv > 60) && (blv <= 70)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/60.png"));
            else if((blv > 70) && (blv <= 80)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/70.png"));
            else if((blv > 80) && (blv <= 90)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/80.png"));
            else if((blv > 90) && (blv < 100)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/90.png"));
            else if(blv == 100) this->batteryImage->SetImage(gsets.PathForResource("/Battery/100.png"));
            this->batteryText->SetText(std::to_string(blv) + "%");
            this->preblv = blv;
        }
        if((this->preisch != isch) || this->vfirst)
        {
            if(isch) this->batteryChargeImage->SetVisible(true);
            else this->batteryChargeImage->SetVisible(false);
            this->preisch = isch;
        }
        if((this->pretime != dtime) || this->vfirst)
        {
            this->timeText->SetText(dtime);
            this->pretime = dtime;
        }
        if(this->vfirst) this->vfirst = false;
        this->hasusb = usb::IsStatePlugged();
        this->usbImage->SetVisible(this->hasusb);
        u32 connstr = 0;
        Result rc = nifmGetInternetConnectionStatus(NULL, &connstr, NULL);
        std::string connimg = "None";
        if(rc == 0) if(connstr > 0) connimg = std::to_string(connstr);
        if(connstr != this->connstate)
        {
            this->connImage->SetImage(gsets.PathForResource("/Connection/" + connimg + ".png"));
            this->connstate = connstr;
        }
    }

    void MainApplication::LoadMenuData(std::string Name, std::string ImageName, std::string TempHead, bool CommonIcon)
    {
        if(this->menuImage != NULL)
        {
            this->menuImage->SetVisible(true);
            if(CommonIcon) this->menuImage->SetImage(gsets.PathForResource("/Common/" + ImageName + ".png"));
            else this->menuImage->SetImage(ImageName);
        }
        if(this->menuNameText != NULL)
        {
            this->menuNameText->SetVisible(true);
            this->menuNameText->SetText(Name);
        }
        if(this->menuHeadText != NULL)
        {
            this->menuHeadText->SetVisible(true);
            this->LoadMenuHead(TempHead);
        }
    }

    void MainApplication::LoadMenuHead(std::string Head)
    {
        if(this->menuHeadText != NULL) this->menuHeadText->SetText(Head);
    }

    void MainApplication::UnloadMenuData()
    {
        this->menuImage->SetVisible(false);
        this->menuNameText->SetVisible(false);
        this->menuHeadText->SetVisible(false);
    }

    void MainApplication::browser_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            if(this->browser->GoBack()) this->browser->UpdateElements();
            else
            {
                this->UnloadMenuData();
                this->LoadLayout(this->mainMenu);
            }
        }
        else if(Down & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = this->browser->GetExplorer()->IsDirectory(clipboard);
                std::string fsicon;
                if(cdir) fsicon = gsets.PathForResource("/FileSystem/Directory.png");
                else
                {
                    std::string ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") fsicon = gsets.PathForResource("/FileSystem/NSP.png");
                    else if(ext == "nro") fsicon = gsets.PathForResource("/FileSystem/NRO.png");
                    else if(ext == "tik") fsicon = gsets.PathForResource("/FileSystem/TIK.png");
                    else if(ext == "cert") fsicon = gsets.PathForResource("/FileSystem/CERT.png");
                    else if(ext == "nca") fsicon = gsets.PathForResource("/FileSystem/NCA.png");
                    else if(ext == "nxtheme") fsicon = gsets.PathForResource("/FileSystem/NXTheme.png");
                    else fsicon = gsets.PathForResource("/FileSystem/File.png");
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(222), set::GetDictionaryEntry(223) + "\n(" + clipboard + ")", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true, fsicon);
                if(sopt == 0)
                {
                    std::string cname = fs::GetFileName(clipboard);
                    mainapp->LoadLayout(mainapp->GetCopyLayout());
                    mainapp->GetCopyLayout()->StartCopy(clipboard, this->browser->GetExplorer()->FullPathFor(cname), cdir, this->browser->GetExplorer(), this->browser);
                    this->browser->UpdateElements();
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter(set::GetDictionaryEntry(224));
        }
        else if(Down & KEY_L)
        {
            std::string cfile = AskForText(set::GetDictionaryEntry(225), "");
            if(cfile != "")
            {
                std::string ffile = this->browser->GetExplorer()->FullPathFor(cfile);
                std::string pffile = this->browser->GetExplorer()->FullPresentablePathFor(cfile);
                if(this->browser->GetExplorer()->IsFile(ffile) || this->browser->GetExplorer()->IsDirectory(ffile)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(255));
                else
                {
                    this->browser->GetExplorer()->CreateFile(ffile);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(227) + " \'" + pffile + "\'");
                    this->browser->UpdateElements();
                }
            }
        }
        else if(Down & KEY_R)
        {
            std::string cdir = AskForText(set::GetDictionaryEntry(250), "");
            if(cdir != "")
            {
                std::string fdir = this->browser->GetExplorer()->FullPathFor(cdir);
                std::string pfdir = this->browser->GetExplorer()->FullPresentablePathFor(cdir);
                if(this->browser->GetExplorer()->IsFile(fdir) || this->browser->GetExplorer()->IsDirectory(fdir)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(255));
                else
                {
                    this->browser->GetExplorer()->CreateDirectory(fdir);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(228) + " \'" + pfdir + "\'");
                    this->browser->UpdateElements();
                }
            }
        }
    }

    void MainApplication::usbInstall_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::fileContent_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B) this->LoadLayout(this->fileContent->GetPreviousLayout());
        else if((Down & KEY_DDOWN) || (Down & KEY_LSTICK_DOWN) || (Held & KEY_RSTICK_DOWN)) this->fileContent->ScrollDown();
        else if((Down & KEY_DUP) || (Down & KEY_LSTICK_UP) || (Held & KEY_RSTICK_UP)) this->fileContent->ScrollUp();
    }

    void MainApplication::contentInformation_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            mainapp->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(189));
            this->LoadLayout(this->storageContents);
        }
    }

    void MainApplication::storageContents_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            mainapp->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(33));
            this->LoadLayout(this->contentManager);
        }
    }

    void MainApplication::contentManager_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::ticketManager_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::account_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->account->CleanData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::sysInfo_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::about_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::OnInput(u64 Down, u64 Up, u64 Held)
    {
        if(((Down & KEY_PLUS) || (Down & KEY_MINUS)) && IsNRO()) this->Close();
        else if((Down & KEY_ZL) || (Down & KEY_ZR)) ShowPowerTasksDialog(set::GetDictionaryEntry(229), set::GetDictionaryEntry(230));
    }

    MainMenuLayout *MainApplication::GetMainMenuLayout()
    {
        return this->mainMenu;
    }

    PartitionBrowserLayout *MainApplication::GetBrowserLayout()
    {
        return this->browser;
    }

    FileContentLayout *MainApplication::GetFileContentLayout()
    {
        return this->fileContent;
    }

    CopyLayout *MainApplication::GetCopyLayout()
    {
        return this->copy;
    }

    InstallLayout *MainApplication::GetInstallLayout()
    {
        return this->nspInstall;
    }

    USBInstallLayout *MainApplication::GetUSBInstallLayout()
    {
        return this->usbInstall;
    }

    ContentInformationLayout *MainApplication::GetContentInformationLayout()
    {
        return this->contentInformation;
    }

    StorageContentsLayout *MainApplication::GetStorageContentsLayout()
    {
        return this->storageContents;
    }

    ContentManagerLayout *MainApplication::GetContentManagerLayout()
    {
        return this->contentManager;
    }

    TitleDumperLayout *MainApplication::GetTitleDumperLayout()
    {
        return this->titleDump;
    }

    TicketManagerLayout *MainApplication::GetTicketManagerLayout()
    {
        return this->ticketManager;
    }

    AccountLayout *MainApplication::GetAccountLayout()
    {
        return this->account;
    }

    SystemInfoLayout *MainApplication::GetSystemInfoLayout()
    {
        return this->sysInfo;
    }

    UpdateLayout *MainApplication::GetUpdateLayout()
    {
        return this->update;
    }

    AboutLayout *MainApplication::GetAboutLayout()
    {
        return this->about;
    }

    void UpdateClipboard(std::string Path)
    {
        SetClipboard(Path);
        std::string copymsg;
        if(mainapp->GetBrowserLayout()->GetExplorer()->IsFile(Path)) copymsg = set::GetDictionaryEntry(257);
        else copymsg = set::GetDictionaryEntry(258);
        mainapp->UpdateFooter(copymsg);
    }

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}