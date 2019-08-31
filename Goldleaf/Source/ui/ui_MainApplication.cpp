#include <ui/ui_MainApplication.hpp>
#include <arpa/inet.h>

extern set::Settings gsets;

namespace ui
{
    MainApplication::Ref mainapp;

    extern pu::String clipboard;

    MainApplication::MainApplication() : pu::ui::Application()
    {
        gsets = set::ProcessSettings();
        set::Initialize();
        if(acc::SelectFromPreselectedUser()) acc::CacheSelectedUserIcon();
        pu::ui::render::SetDefaultFont(gsets.PathForResource("/Roboto-Medium.ttf"));
        this->preblv = 0;
        this->seluser = 0;
        this->preisch = false;
        this->pretime = "";
        this->vfirst = true;
        this->connstate = 0;
        this->baseImage = pu::ui::elm::Image::New(0, 0, gsets.PathForResource("/Base.png"));
        this->timeText = pu::ui::elm::TextBlock::New(1124, 15, "00:00:00");
        this->timeText->SetColor(gsets.CustomScheme.Text);
        this->batteryText = pu::ui::elm::TextBlock::New(1015, 20, "0%", 20);
        this->batteryText->SetColor(gsets.CustomScheme.Text);
        this->batteryImage = pu::ui::elm::Image::New(960, 8, gsets.PathForResource("/Battery/0.png"));
        this->batteryChargeImage = pu::ui::elm::Image::New(960, 8, gsets.PathForResource("/Battery/Charge.png"));
        this->menuBanner = pu::ui::elm::Image::New(10, 62, gsets.PathForResource("/MenuBanner.png"));
        this->menuImage = pu::ui::elm::Image::New(15, 69, gsets.PathForResource("/Common/SdCard.png"));
        this->menuImage->SetWidth(85);
        this->menuImage->SetHeight(85);
        this->userImage = ClickableImage::New(1090, 75, gsets.PathForResource("/Common/User.png"));
        this->userImage->SetWidth(70);
        this->userImage->SetHeight(70);
        this->userImage->SetOnClick(std::bind(&MainApplication::userImage_OnClick, this));
        this->helpImage = ClickableImage::New(1180, 80, gsets.PathForResource("/Common/Help.png"));
        this->helpImage->SetWidth(60);
        this->helpImage->SetHeight(60);
        this->helpImage->SetOnClick(std::bind(&MainApplication::helpImage_OnClick, this));
        this->usbImage = pu::ui::elm::Image::New(695, 12, gsets.PathForResource("/Common/USB.png"));
        this->usbImage->SetWidth(40);
        this->usbImage->SetHeight(40);
        this->usbImage->SetVisible(false);
        this->connImage = pu::ui::elm::Image::New(755, 12, gsets.PathForResource("/Connection/None.png"));
        this->connImage->SetWidth(40);
        this->connImage->SetHeight(40);
        this->connImage->SetVisible(true);
        this->ipText = pu::ui::elm::TextBlock::New(800, 20, "", 20);
        this->ipText->SetColor(gsets.CustomScheme.Text);
        this->menuNameText = pu::ui::elm::TextBlock::New(120, 85, "-");
        this->menuNameText->SetColor(gsets.CustomScheme.Text);
        this->menuHeadText = pu::ui::elm::TextBlock::New(120, 120, "-", 20);
        this->menuHeadText->SetColor(gsets.CustomScheme.Text);
        this->UnloadMenuData();
        this->toast = pu::ui::extras::Toast::New(":", 20, pu::ui::Color(225, 225, 225, 255), pu::ui::Color(40, 40, 40, 255));
        this->UpdateValues();
        this->mainMenu = MainMenuLayout::New();
        this->browser = PartitionBrowserLayout::New();
        this->browser->SetOnInput(std::bind(&MainApplication::browser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->fileContent = FileContentLayout::New();
        this->fileContent->SetOnInput(std::bind(&MainApplication::fileContent_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->copy = CopyLayout::New();
        this->exploreMenu = ExploreMenuLayout::New();
        this->exploreMenu->SetOnInput(std::bind(&MainApplication::exploreMenu_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->pcExplore = PCExploreLayout::New();
        this->pcExplore->SetOnInput(std::bind(&MainApplication::pcExplore_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->usbDrives = USBDrivesLayout::New();
        this->usbDrives->SetOnInput(std::bind(&MainApplication::usbDrives_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->nspInstall = InstallLayout::New();
        this->contentInformation = ContentInformationLayout::New();
        this->contentInformation->SetOnInput(std::bind(&MainApplication::contentInformation_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->storageContents = StorageContentsLayout::New();
        this->storageContents->SetOnInput(std::bind(&MainApplication::storageContents_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->contentManager = ContentManagerLayout::New();
        this->contentManager->SetOnInput(std::bind(&MainApplication::contentManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->titleDump = TitleDumperLayout::New();
        this->unusedTickets = UnusedTicketsLayout::New();
        this->unusedTickets->SetOnInput(std::bind(&MainApplication::unusedTickets_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->account = AccountLayout::New();
        this->account->SetOnInput(std::bind(&MainApplication::account_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->amiibo = AmiiboDumpLayout::New();
        this->amiibo->SetOnInput(std::bind(&MainApplication::amiibo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sysInfo = SystemInfoLayout::New();
        this->sysInfo->SetOnInput(std::bind(&MainApplication::sysInfo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->update = UpdateLayout::New();
        this->about = AboutLayout::New();
        this->about->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->mainMenu->SetBackgroundColor(gsets.CustomScheme.Background);
        this->browser->SetBackgroundColor(gsets.CustomScheme.Background);
        this->exploreMenu->SetBackgroundColor(gsets.CustomScheme.Background);
        this->pcExplore->SetBackgroundColor(gsets.CustomScheme.Background);
        this->usbDrives->SetBackgroundColor(gsets.CustomScheme.Background);
        this->fileContent->SetBackgroundColor(gsets.CustomScheme.Background);
        this->copy->SetBackgroundColor(gsets.CustomScheme.Background);
        this->nspInstall->SetBackgroundColor(gsets.CustomScheme.Background);
        this->contentInformation->SetBackgroundColor(gsets.CustomScheme.Background);
        this->storageContents->SetBackgroundColor(gsets.CustomScheme.Background);
        this->contentManager->SetBackgroundColor(gsets.CustomScheme.Background);
        this->titleDump->SetBackgroundColor(gsets.CustomScheme.Background);
        this->unusedTickets->SetBackgroundColor(gsets.CustomScheme.Background);
        this->account->SetBackgroundColor(gsets.CustomScheme.Background);
        this->amiibo->SetBackgroundColor(gsets.CustomScheme.Background);
        this->sysInfo->SetBackgroundColor(gsets.CustomScheme.Background);
        this->update->SetBackgroundColor(gsets.CustomScheme.Background);
        this->about->SetBackgroundColor(gsets.CustomScheme.Background);
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
        this->unusedTickets->Add(this->baseImage);
        this->account->Add(this->baseImage);
        this->amiibo->Add(this->baseImage);
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
        this->unusedTickets->Add(this->timeText);
        this->account->Add(this->timeText);
        this->amiibo->Add(this->timeText);
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
        this->unusedTickets->Add(this->batteryText);
        this->account->Add(this->batteryText);
        this->amiibo->Add(this->batteryText);
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
        this->unusedTickets->Add(this->batteryImage);
        this->account->Add(this->batteryImage);
        this->amiibo->Add(this->batteryImage);
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
        this->unusedTickets->Add(this->batteryChargeImage);
        this->account->Add(this->batteryChargeImage);
        this->amiibo->Add(this->batteryChargeImage);
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
        this->unusedTickets->Add(this->menuImage);
        this->account->Add(this->menuImage);
        this->amiibo->Add(this->menuImage);
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
        this->unusedTickets->Add(this->usbImage);
        this->account->Add(this->usbImage);
        this->amiibo->Add(this->usbImage);
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
        this->unusedTickets->Add(this->connImage);
        this->account->Add(this->connImage);
        this->amiibo->Add(this->connImage);
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
        this->unusedTickets->Add(this->ipText);
        this->account->Add(this->ipText);
        this->amiibo->Add(this->ipText);
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
        this->unusedTickets->Add(this->menuNameText);
        this->account->Add(this->menuNameText);
        this->amiibo->Add(this->menuNameText);
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
        this->unusedTickets->Add(this->menuHeadText);
        this->account->Add(this->menuHeadText);
        this->amiibo->Add(this->menuHeadText);
        this->sysInfo->Add(this->menuHeadText);
        this->update->Add(this->menuHeadText);
        this->about->Add(this->menuHeadText);
        this->mainMenu->Add(this->userImage);
        this->browser->Add(this->userImage);
        this->exploreMenu->Add(this->userImage);
        this->pcExplore->Add(this->userImage);
        this->usbDrives->Add(this->userImage);
        this->fileContent->Add(this->userImage);
        this->copy->Add(this->userImage);
        this->nspInstall->Add(this->userImage);
        this->contentInformation->Add(this->userImage);
        this->storageContents->Add(this->userImage);
        this->contentManager->Add(this->userImage);
        this->titleDump->Add(this->userImage);
        this->unusedTickets->Add(this->userImage);
        this->account->Add(this->userImage);
        this->amiibo->Add(this->userImage);
        this->sysInfo->Add(this->userImage);
        this->update->Add(this->userImage);
        this->about->Add(this->userImage);
        this->mainMenu->Add(this->helpImage);
        this->browser->Add(this->helpImage);
        this->exploreMenu->Add(this->helpImage);
        this->pcExplore->Add(this->helpImage);
        this->usbDrives->Add(this->helpImage);
        this->fileContent->Add(this->helpImage);
        this->copy->Add(this->helpImage);
        this->nspInstall->Add(this->helpImage);
        this->contentInformation->Add(this->helpImage);
        this->storageContents->Add(this->helpImage);
        this->contentManager->Add(this->helpImage);
        this->titleDump->Add(this->helpImage);
        this->unusedTickets->Add(this->helpImage);
        this->account->Add(this->helpImage);
        this->amiibo->Add(this->helpImage);
        this->sysInfo->Add(this->helpImage);
        this->update->Add(this->helpImage);
        this->about->Add(this->helpImage);
        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->LoadLayout(this->mainMenu);   
        this->welcomeshown = false;
        this->start = std::chrono::steady_clock::now();
    }

    void MainApplication::ShowNotification(pu::String Text)
    {
        this->EndOverlay();
        this->toast->SetText(Text);
        this->StartOverlayWithTimeout(this->toast, 1500);
    }

    void MainApplication::UpdateValues()
    {
        if(!this->welcomeshown)
        {
            auto tnow = std::chrono::steady_clock::now();
            auto timediff = std::chrono::duration_cast<std::chrono::milliseconds>(tnow - this->start).count();
            if(timediff >= 1000)
            {
                this->ShowNotification(u"Welcome to Goldleaf! Press + anytime for help and control information.");
                this->welcomeshown = true;
            }
        }

        pu::String dtime = hos::GetCurrentTime();
        u32 blv = hos::GetBatteryLevel();
        bool isch = hos::IsCharging();
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
            this->connImage->SetWidth(40);
            this->connImage->SetHeight(40);
            this->connstate = connstr;
        }
        if(connstr > 0)
        {
            u32 ip = gethostid();
            char sip[0x20] = {0};
            inet_ntop(AF_INET, &ip, sip, 0x20);
            this->ipText->SetText(pu::String(sip));
        }
        else this->ipText->SetText("");
        auto user = acc::GetSelectedUser();
        if(user != this->seluser)
        {
            this->seluser = user;
            if(this->seluser == 0) this->userImage->SetImage(gsets.PathForResource("/Common/User.png"));
            else
            {
                auto usericon = acc::GetCachedUserIcon();
                if(fs::Exists(usericon)) this->userImage->SetImage(usericon);
                else this->userImage->SetImage(gsets.PathForResource("/Common/User.png"));
            }
            this->userImage->SetWidth(70);
            this->userImage->SetHeight(70);
        }
    }

    void MainApplication::LoadMenuData(pu::String Name, std::string ImageName, pu::String TempHead, bool CommonIcon)
    {
        if(this->menuImage != NULL)
        {
            this->menuImage->SetVisible(true);
            if(CommonIcon) this->menuImage->SetImage(gsets.PathForResource("/Common/" + ImageName + ".png"));
            else this->menuImage->SetImage(ImageName);
            this->menuImage->SetWidth(85);
            this->menuImage->SetHeight(85);
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

    void MainApplication::LoadMenuHead(pu::String Head)
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
            if(this->browser->GoBack()) this->browser->UpdateElements(-1);
            else
            {
                this->UnloadMenuData();
                this->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
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
                    pu::String ext = fs::GetExtension(clipboard);
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
                    pu::String cname = fs::GetFileName(clipboard);
                    this->LoadLayout(this->GetCopyLayout());
                    this->GetCopyLayout()->StartCopy(clipboard, this->browser->GetExplorer()->FullPathFor(cname), cdir, this->browser->GetExplorer());
                    mainapp->LoadLayout(this->browser);
                    this->browser->UpdateElements();
                    clipboard = "";
                }
            }
            else this->ShowNotification(set::GetDictionaryEntry(224));
        }
        else if(Down & KEY_L)
        {
            pu::String cfile = AskForText(set::GetDictionaryEntry(225), "");
            if(cfile != "")
            {
                pu::String ffile = this->browser->GetExplorer()->FullPathFor(cfile);
                pu::String pffile = this->browser->GetExplorer()->FullPresentablePathFor(cfile);
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
            pu::String cdir = AskForText(set::GetDictionaryEntry(250), "");
            if(cdir != "")
            {
                pu::String fdir = this->browser->GetExplorer()->FullPathFor(cdir);
                pu::String pfdir = this->browser->GetExplorer()->FullPresentablePathFor(cdir);
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
            this->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
            this->LoadLayout(this->exploreMenu);
        }
    }

    void MainApplication::usbDrives_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
            this->LoadLayout(this->exploreMenu);
        }
    }

    void MainApplication::fileContent_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B) this->LoadLayout(this->browser);
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

    void MainApplication::unusedTickets_Input(u64 Down, u64 Up, u64 Held)
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
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::amiibo_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
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

    void MainApplication::userImage_OnClick()
    {
        if(acc::SelectUser())
        {
            acc::CacheSelectedUserIcon();
            this->ShowNotification(set::GetDictionaryEntry(324));
        }
    }

    void MainApplication::helpImage_OnClick()
    {
        this->CreateShowDialog(set::GetDictionaryEntry(162), set::GetDictionaryEntry(342) + "\n\n" + set::GetDictionaryEntry(343) + "\n" + set::GetDictionaryEntry(344) + "\n" + set::GetDictionaryEntry(345) + "\n" + set::GetDictionaryEntry(346) + "\n" + set::GetDictionaryEntry(347), {set::GetDictionaryEntry(234)}, false);
    }

    void MainApplication::OnInput(u64 Down, u64 Up, u64 Held)
    {
        if((Down & KEY_MINUS) && (GetLaunchMode() == LaunchMode::Applet))
        {
            this->CloseWithFadeOut();
        }
        else if((Down & KEY_ZL) || (Down & KEY_ZR)) ShowPowerTasksDialog(set::GetDictionaryEntry(229), set::GetDictionaryEntry(230));
        else if(Down & KEY_PLUS) this->helpImage_OnClick();
    }

    MainMenuLayout::Ref &MainApplication::GetMainMenuLayout()
    {
        return this->mainMenu;
    }

    PartitionBrowserLayout::Ref &MainApplication::GetBrowserLayout()
    {
        return this->browser;
    }

    FileContentLayout::Ref &MainApplication::GetFileContentLayout()
    {
        return this->fileContent;
    }

    CopyLayout::Ref &MainApplication::GetCopyLayout()
    {
        return this->copy;
    }

    ExploreMenuLayout::Ref &MainApplication::GetExploreMenuLayout()
    {
        return this->exploreMenu;
    }

    PCExploreLayout::Ref &MainApplication::GetPCExploreLayout()
    {
        return this->pcExplore;
    }

    USBDrivesLayout::Ref &MainApplication::GetUSBDrivesLayout()
    {
        return this->usbDrives;
    }

    InstallLayout::Ref &MainApplication::GetInstallLayout()
    {
        return this->nspInstall;
    }

    ContentInformationLayout::Ref &MainApplication::GetContentInformationLayout()
    {
        return this->contentInformation;
    }

    StorageContentsLayout::Ref &MainApplication::GetStorageContentsLayout()
    {
        return this->storageContents;
    }

    ContentManagerLayout::Ref &MainApplication::GetContentManagerLayout()
    {
        return this->contentManager;
    }

    TitleDumperLayout::Ref &MainApplication::GetTitleDumperLayout()
    {
        return this->titleDump;
    }

    UnusedTicketsLayout::Ref &MainApplication::GetUnusedTicketsLayout()
    {
        return this->unusedTickets;
    }

    AccountLayout::Ref &MainApplication::GetAccountLayout()
    {
        return this->account;
    }

    AmiiboDumpLayout::Ref &MainApplication::GetAmiiboDumpLayout()
    {
        return this->amiibo;
    }

    SystemInfoLayout::Ref &MainApplication::GetSystemInfoLayout()
    {
        return this->sysInfo;
    }

    UpdateLayout::Ref &MainApplication::GetUpdateLayout()
    {
        return this->update;
    }

    AboutLayout::Ref &MainApplication::GetAboutLayout()
    {
        return this->about;
    }

    void UpdateClipboard(pu::String Path)
    {
        SetClipboard(Path);
        pu::String copymsg;
        if(mainapp->GetBrowserLayout()->GetExplorer()->IsFile(Path)) copymsg = set::GetDictionaryEntry(257);
        else copymsg = set::GetDictionaryEntry(258);
        mainapp->ShowNotification(copymsg);
    }
}