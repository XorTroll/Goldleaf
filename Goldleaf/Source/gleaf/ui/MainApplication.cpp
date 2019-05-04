#include <gleaf/ui/MainApplication.hpp>
#include <gleaf/ui/Utils.hpp>
#include <arpa/inet.h>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    MainApplication *mainapp;
    extern std::string clipboard;

    MainApplication::MainApplication(StartMode Mode) : pu::Application()
    {
        this->stmode = Mode;
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
        this->menuImage = new pu::element::Image(15, 69, gsets.PathForResource("/Common/SdCard.png"));
        this->menuImage->SetWidth(85);
        this->menuImage->SetHeight(85);
        this->usbImage = new pu::element::Image(710, 12, gsets.PathForResource("/Common/USB.png"));
        this->usbImage->SetWidth(40);
        this->usbImage->SetHeight(40);
        this->usbImage->SetVisible(false);
        this->connImage = new pu::element::Image(755, 12, gsets.PathForResource("/Connection/None.png"));
        this->connImage->SetWidth(40);
        this->connImage->SetHeight(40);
        this->connImage->SetVisible(true);
        this->ipText = new pu::element::TextBlock(800, 22, "127.0.0.1", 20);
        this->ipText->SetColor(gsets.CustomScheme.Text);
        this->menuNameText = new pu::element::TextBlock(120, 85, "-");
        this->menuNameText->SetColor(gsets.CustomScheme.Text);
        this->menuHeadText = new pu::element::TextBlock(120, 120, "-", 20);
        this->menuHeadText->SetColor(gsets.CustomScheme.Text);
        this->UnloadMenuData();
        this->toast = new pu::overlay::Toast(":", 20, { 225, 225, 225, 255 }, { 40, 40, 40, 255 });
        this->UpdateValues();
        this->mainMenu = new MainMenuLayout();
        this->browser = new PartitionBrowserLayout();
        this->browser->SetOnInput(std::bind(&MainApplication::browser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->fileContent = new FileContentLayout();
        this->fileContent->SetOnInput(std::bind(&MainApplication::fileContent_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->copy = new CopyLayout();
        this->exploreMenu = new ExploreMenuLayout();
        this->exploreMenu->SetOnInput(std::bind(&MainApplication::exploreMenu_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->pcExplore = new PCExploreLayout();
        this->pcExplore->SetOnInput(std::bind(&MainApplication::pcExplore_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->usbDrives = new USBDrivesLayout();
        this->usbDrives->SetOnInput(std::bind(&MainApplication::usbDrives_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->nspInstall = new InstallLayout();
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
        this->mainMenu->Add(this->baseImage);
        this->browser->Add(this->baseImage);
        this->exploreMenu->Add(this->baseImage);
        this->pcExplore->Add(this->baseImage);
        this->usbDrives->Add(this->baseImage);
        this->fileContent->Add(this->baseImage);
        this->copy->Add(this->baseImage);
        this->nspInstall->Add(this->baseImage);
        this->contentInformation->Add(this->baseImage);
        this->storageContents->Add(this->baseImage);
        this->contentManager->Add(this->baseImage);
        this->titleDump->Add(this->baseImage);
        this->ticketManager->Add(this->baseImage);
        this->account->Add(this->baseImage);
        this->sysInfo->Add(this->baseImage);
        this->update->Add(this->baseImage);
        this->about->Add(this->baseImage);
        this->mainMenu->Add(this->timeText);
        this->browser->Add(this->timeText);
        this->exploreMenu->Add(this->timeText);
        this->pcExplore->Add(this->timeText);
        this->usbDrives->Add(this->timeText);
        this->fileContent->Add(this->timeText);
        this->copy->Add(this->timeText);
        this->nspInstall->Add(this->timeText);
        this->contentInformation->Add(this->timeText);
        this->storageContents->Add(this->timeText);
        this->contentManager->Add(this->timeText);
        this->titleDump->Add(this->timeText);
        this->ticketManager->Add(this->timeText);
        this->account->Add(this->timeText);
        this->sysInfo->Add(this->timeText);
        this->update->Add(this->timeText);
        this->about->Add(this->timeText);
        this->mainMenu->Add(this->batteryText);
        this->browser->Add(this->batteryText);
        this->exploreMenu->Add(this->batteryText);
        this->pcExplore->Add(this->batteryText);
        this->usbDrives->Add(this->batteryText);
        this->fileContent->Add(this->batteryText);
        this->copy->Add(this->batteryText);
        this->nspInstall->Add(this->batteryText);
        this->contentInformation->Add(this->batteryText);
        this->storageContents->Add(this->batteryText);
        this->contentManager->Add(this->batteryText);
        this->titleDump->Add(this->batteryText);
        this->ticketManager->Add(this->batteryText);
        this->account->Add(this->batteryText);
        this->sysInfo->Add(this->batteryText);
        this->update->Add(this->batteryText);
        this->about->Add(this->batteryText);
        this->mainMenu->Add(this->batteryImage);
        this->browser->Add(this->batteryImage);
        this->exploreMenu->Add(this->batteryImage);
        this->pcExplore->Add(this->batteryImage);
        this->usbDrives->Add(this->batteryImage);
        this->fileContent->Add(this->batteryImage);
        this->copy->Add(this->batteryImage);
        this->nspInstall->Add(this->batteryImage);
        this->contentInformation->Add(this->batteryImage);
        this->storageContents->Add(this->batteryImage);
        this->contentManager->Add(this->batteryImage);
        this->titleDump->Add(this->batteryImage);
        this->ticketManager->Add(this->batteryImage);
        this->account->Add(this->batteryImage);
        this->sysInfo->Add(this->batteryImage);
        this->update->Add(this->batteryImage);
        this->about->Add(this->batteryImage);
        this->mainMenu->Add(this->batteryChargeImage);
        this->browser->Add(this->batteryChargeImage);
        this->exploreMenu->Add(this->batteryChargeImage);
        this->pcExplore->Add(this->batteryChargeImage);
        this->usbDrives->Add(this->batteryChargeImage);
        this->fileContent->Add(this->batteryChargeImage);
        this->copy->Add(this->batteryChargeImage);
        this->nspInstall->Add(this->batteryChargeImage);
        this->contentInformation->Add(this->batteryChargeImage);
        this->storageContents->Add(this->batteryChargeImage);
        this->contentManager->Add(this->batteryChargeImage);
        this->titleDump->Add(this->batteryChargeImage);
        this->ticketManager->Add(this->batteryChargeImage);
        this->account->Add(this->batteryChargeImage);
        this->sysInfo->Add(this->batteryChargeImage);
        this->update->Add(this->batteryChargeImage);
        this->about->Add(this->batteryChargeImage);
        this->mainMenu->Add(this->menuImage);
        this->browser->Add(this->menuImage);
        this->exploreMenu->Add(this->menuImage);
        this->pcExplore->Add(this->menuImage);
        this->usbDrives->Add(this->menuImage);
        this->fileContent->Add(this->menuImage);
        this->copy->Add(this->menuImage);
        this->nspInstall->Add(this->menuImage);
        this->contentInformation->Add(this->menuImage);
        this->storageContents->Add(this->menuImage);
        this->contentManager->Add(this->menuImage);
        this->titleDump->Add(this->menuImage);
        this->ticketManager->Add(this->menuImage);
        this->account->Add(this->menuImage);
        this->sysInfo->Add(this->menuImage);
        this->update->Add(this->menuImage);
        this->about->Add(this->menuImage);
        this->mainMenu->Add(this->usbImage);
        this->browser->Add(this->usbImage);
        this->exploreMenu->Add(this->usbImage);
        this->pcExplore->Add(this->usbImage);
        this->usbDrives->Add(this->usbImage);
        this->fileContent->Add(this->usbImage);
        this->copy->Add(this->usbImage);
        this->nspInstall->Add(this->usbImage);
        this->contentInformation->Add(this->usbImage);
        this->storageContents->Add(this->usbImage);
        this->contentManager->Add(this->usbImage);
        this->titleDump->Add(this->usbImage);
        this->ticketManager->Add(this->usbImage);
        this->account->Add(this->usbImage);
        this->sysInfo->Add(this->usbImage);
        this->update->Add(this->usbImage);
        this->about->Add(this->usbImage);
        this->mainMenu->Add(this->connImage);
        this->browser->Add(this->connImage);
        this->exploreMenu->Add(this->connImage);
        this->pcExplore->Add(this->connImage);
        this->usbDrives->Add(this->connImage);
        this->fileContent->Add(this->connImage);
        this->copy->Add(this->connImage);
        this->nspInstall->Add(this->connImage);
        this->contentInformation->Add(this->connImage);
        this->storageContents->Add(this->connImage);
        this->contentManager->Add(this->connImage);
        this->titleDump->Add(this->connImage);
        this->ticketManager->Add(this->connImage);
        this->account->Add(this->connImage);
        this->sysInfo->Add(this->connImage);
        this->update->Add(this->connImage);
        this->about->Add(this->connImage);
        this->mainMenu->Add(this->ipText);
        this->browser->Add(this->ipText);
        this->exploreMenu->Add(this->ipText);
        this->pcExplore->Add(this->ipText);
        this->usbDrives->Add(this->ipText);
        this->fileContent->Add(this->ipText);
        this->copy->Add(this->ipText);
        this->nspInstall->Add(this->ipText);
        this->contentInformation->Add(this->ipText);
        this->storageContents->Add(this->ipText);
        this->contentManager->Add(this->ipText);
        this->titleDump->Add(this->ipText);
        this->ticketManager->Add(this->ipText);
        this->account->Add(this->ipText);
        this->sysInfo->Add(this->ipText);
        this->update->Add(this->ipText);
        this->about->Add(this->ipText);
        this->mainMenu->Add(this->menuBanner);
        this->mainMenu->Add(this->menuNameText);
        this->browser->Add(this->menuNameText);
        this->exploreMenu->Add(this->menuNameText);
        this->pcExplore->Add(this->menuNameText);
        this->usbDrives->Add(this->menuNameText);
        this->fileContent->Add(this->menuNameText);
        this->copy->Add(this->menuNameText);
        this->nspInstall->Add(this->menuNameText);
        this->contentInformation->Add(this->menuNameText);
        this->storageContents->Add(this->menuNameText);
        this->contentManager->Add(this->menuNameText);
        this->titleDump->Add(this->menuNameText);
        this->ticketManager->Add(this->menuNameText);
        this->account->Add(this->menuNameText);
        this->sysInfo->Add(this->menuNameText);
        this->update->Add(this->menuNameText);
        this->about->Add(this->menuNameText);
        this->mainMenu->Add(this->menuHeadText);
        this->browser->Add(this->menuHeadText);
        this->exploreMenu->Add(this->menuHeadText);
        this->pcExplore->Add(this->menuHeadText);
        this->usbDrives->Add(this->menuHeadText);
        this->fileContent->Add(this->menuHeadText);
        this->copy->Add(this->menuHeadText);
        this->nspInstall->Add(this->menuHeadText);
        this->contentInformation->Add(this->menuHeadText);
        this->storageContents->Add(this->menuHeadText);
        this->contentManager->Add(this->menuHeadText);
        this->titleDump->Add(this->menuHeadText);
        this->ticketManager->Add(this->menuHeadText);
        this->account->Add(this->menuHeadText);
        this->sysInfo->Add(this->menuHeadText);
        this->update->Add(this->menuHeadText);
        this->about->Add(this->menuHeadText);
        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        switch(this->stmode)
        {
            case StartMode::Normal:
                this->LoadLayout(this->mainMenu);
                break;
            case StartMode::Qlaunch:
                this->LoadLayout(this->about);
                break;
            case StartMode::HomebrewMenu:
                this->LoadLayout(this->about);
                break;
        }
        this->updshown = false;
        this->start = std::chrono::steady_clock::now();
    }

    MainApplication::~MainApplication()
    {
        delete this->baseImage;
        delete this->timeText;
        delete this->batteryText;
        delete this->batteryImage;
        delete this->batteryChargeImage;
        delete this->menuBanner;
        delete this->menuImage;
        delete this->usbImage;
        delete this->connImage;
        delete this->ipText;
        delete this->menuNameText;
        delete this->menuHeadText;
        delete this->toast;
        delete this->mainMenu;
        delete this->browser;
        delete this->fileContent;
        delete this->copy;
        delete this->exploreMenu;
        delete this->pcExplore;
        delete this->usbDrives;
        delete this->nspInstall;
        delete this->contentInformation;
        delete this->storageContents;
        delete this->contentManager;
        delete this->titleDump;
        delete this->ticketManager;
        delete this->account;
        delete this->sysInfo;
        delete this->update;
        delete this->about;
    }

    void MainApplication::ShowNotification(std::string Text)
    {
        mainapp->EndOverlay();
        this->toast->SetText(Text);
        mainapp->StartOverlayWithTimeout(this->toast, 1500);
    }

    void MainApplication::UpdateValues()
    {
        auto ct = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ct - this->start).count();
        if((diff >= 500) && (!this->updshown))
        {
            if(net::CheckVersionDiff()) mainapp->ShowNotification("New Goldleaf updates were found. Go to the updates section to update Goldleaf.");
            this->updshown = true;
        }
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
        if(connstr > 0)
        {
            u32 ip = gethostid();
            char sip[256];
            inet_ntop(AF_INET, &ip, sip, 256);
            this->ipText->SetText(std::string(sip));
        }
        else this->ipText->SetText("<no connection>");
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
                this->LoadMenuData("Mounted content", "Storage", "Explore mounted contents");
                this->LoadLayout(this->exploreMenu);
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
                int sopt = this->CreateShowDialog(set::GetDictionaryEntry(222), set::GetDictionaryEntry(223) + "\n(" + clipboard + ")", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true, fsicon);
                if(sopt == 0)
                {
                    std::string cname = fs::GetFileName(clipboard);
                    this->LoadLayout(this->GetCopyLayout());
                    this->GetCopyLayout()->StartCopy(clipboard, this->browser->GetExplorer()->FullPathFor(cname), cdir, this->browser->GetExplorer(), this->browser);
                    this->browser->UpdateElements();
                    clipboard = "";
                }
            }
            else this->ShowNotification(set::GetDictionaryEntry(224));
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
                    this->ShowNotification(set::GetDictionaryEntry(227) + " \'" + pffile + "\'");
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
                    this->ShowNotification(set::GetDictionaryEntry(228) + " \'" + pfdir + "\'");
                    this->browser->UpdateElements();
                }
            }
        }
    }

    void MainApplication::exploreMenu_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::pcExplore_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadMenuData("Mounted content", "Storage", "Explore mounted contents");
            this->LoadLayout(this->exploreMenu);
        }
    }

    void MainApplication::usbDrives_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadMenuData("Mounted content", "Storage", "Explore mounted contents");
            this->LoadLayout(this->exploreMenu);
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
            this->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(189));
            this->LoadLayout(this->storageContents);
        }
    }

    void MainApplication::storageContents_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(33));
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

    ExploreMenuLayout *MainApplication::GetExploreMenuLayout()
    {
        return this->exploreMenu;
    }

    PCExploreLayout *MainApplication::GetPCExploreLayout()
    {
        return this->pcExplore;
    }

    USBDrivesLayout *MainApplication::GetUSBDrivesLayout()
    {
        return this->usbDrives;
    }

    InstallLayout *MainApplication::GetInstallLayout()
    {
        return this->nspInstall;
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
        mainapp->ShowNotification(copymsg);
    }

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}