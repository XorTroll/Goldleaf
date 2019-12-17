
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

#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern set::Settings global_settings;

namespace ui
{
    extern String clipboard;

    #define MAINAPP_MENU_SET_BASE(layout) { \
        layout->SetBackgroundColor(global_settings.custom_scheme.Background); \
        layout->Add(this->baseImage); \
        layout->Add(this->timeText); \
        layout->Add(this->batteryText); \
        layout->Add(this->batteryImage); \
        layout->Add(this->batteryChargeImage); \
        layout->Add(this->menuImage); \
        layout->Add(this->usbImage); \
        layout->Add(this->connImage); \
        layout->Add(this->ipText); \
        layout->Add(this->menuNameText); \
        layout->Add(this->menuHeadText); \
        layout->Add(this->userImage); \
        layout->Add(this->helpImage); \
    }

    void MainApplication::OnLoad()
    {
        global_settings = set::ProcessSettings();
        set::Initialize();
        if(acc::SelectFromPreselectedUser()) acc::CacheSelectedUserIcon();

        pu::ui::render::SetDefaultFont(global_settings.PathForResource("/Roboto-Medium.ttf"));
        this->preblv = 0;
        this->seluser = {};
        this->preisch = false;
        this->pretime = "";
        this->vfirst = true;
        this->connstate = 0;
        this->baseImage = pu::ui::elm::Image::New(0, 0, global_settings.PathForResource("/Base.png"));
        this->timeText = pu::ui::elm::TextBlock::New(1124, 15, "00:00:00");
        this->timeText->SetColor(global_settings.custom_scheme.Text);
        this->batteryText = pu::ui::elm::TextBlock::New(1015, 20, "0%", 20);
        this->batteryText->SetColor(global_settings.custom_scheme.Text);
        this->batteryImage = pu::ui::elm::Image::New(960, 8, global_settings.PathForResource("/Battery/0.png"));
        this->batteryChargeImage = pu::ui::elm::Image::New(960, 8, global_settings.PathForResource("/Battery/Charge.png"));
        this->menuBanner = pu::ui::elm::Image::New(10, 62, global_settings.PathForResource("/MenuBanner.png"));
        this->menuImage = pu::ui::elm::Image::New(15, 69, global_settings.PathForResource("/Common/SdCard.png"));
        this->menuImage->SetWidth(85);
        this->menuImage->SetHeight(85);
        this->userImage = ClickableImage::New(1090, 75, global_settings.PathForResource("/Common/User.png"));
        this->userImage->SetWidth(70);
        this->userImage->SetHeight(70);
        this->userImage->SetOnClick(std::bind(&MainApplication::userImage_OnClick, this));
        this->helpImage = ClickableImage::New(1180, 80, global_settings.PathForResource("/Common/Help.png"));
        this->helpImage->SetWidth(60);
        this->helpImage->SetHeight(60);
        this->helpImage->SetOnClick(std::bind(&MainApplication::helpImage_OnClick, this));
        this->usbImage = pu::ui::elm::Image::New(695, 12, global_settings.PathForResource("/Common/USB.png"));
        this->usbImage->SetWidth(40);
        this->usbImage->SetHeight(40);
        this->usbImage->SetVisible(false);
        this->connImage = pu::ui::elm::Image::New(755, 12, global_settings.PathForResource("/Connection/None.png"));
        this->connImage->SetWidth(40);
        this->connImage->SetHeight(40);
        this->connImage->SetVisible(true);
        this->ipText = pu::ui::elm::TextBlock::New(800, 20, "", 20);
        this->ipText->SetColor(global_settings.custom_scheme.Text);
        this->menuNameText = pu::ui::elm::TextBlock::New(120, 85, "-");
        this->menuNameText->SetColor(global_settings.custom_scheme.Text);
        this->menuHeadText = pu::ui::elm::TextBlock::New(120, 120, "-", 20);
        this->menuHeadText->SetColor(global_settings.custom_scheme.Text);
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
        this->settings = SettingsLayout::New();
        this->settings->SetOnInput(std::bind(&MainApplication::settings_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->memory = MemoryLayout::New();
        this->memory->SetOnInput(std::bind(&MainApplication::memory_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->update = UpdateLayout::New();
        this->webBrowser = WebBrowserLayout::New();
        this->webBrowser->SetOnInput(std::bind(&MainApplication::webBrowser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->about = AboutLayout::New();
        this->about->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        MAINAPP_MENU_SET_BASE(this->mainMenu);
        MAINAPP_MENU_SET_BASE(this->browser);
        MAINAPP_MENU_SET_BASE(this->exploreMenu);
        MAINAPP_MENU_SET_BASE(this->pcExplore);
        MAINAPP_MENU_SET_BASE(this->fileContent);
        MAINAPP_MENU_SET_BASE(this->copy);
        MAINAPP_MENU_SET_BASE(this->nspInstall);
        MAINAPP_MENU_SET_BASE(this->contentInformation);
        MAINAPP_MENU_SET_BASE(this->storageContents);
        MAINAPP_MENU_SET_BASE(this->contentManager);
        MAINAPP_MENU_SET_BASE(this->titleDump);
        MAINAPP_MENU_SET_BASE(this->unusedTickets);
        MAINAPP_MENU_SET_BASE(this->account);
        MAINAPP_MENU_SET_BASE(this->amiibo);
        MAINAPP_MENU_SET_BASE(this->settings);
        MAINAPP_MENU_SET_BASE(this->memory);
        MAINAPP_MENU_SET_BASE(this->update);
        MAINAPP_MENU_SET_BASE(this->webBrowser);
        MAINAPP_MENU_SET_BASE(this->about);

        // Special extras
        this->mainMenu->Add(this->menuBanner);

        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->LoadLayout(this->mainMenu);
        this->welcomeshown = false;
        this->start = std::chrono::steady_clock::now();
    }

    void MainApplication::ShowNotification(String Text)
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
                this->ShowNotification(set::GetDictionaryEntry(320));
                this->welcomeshown = true;
            }
        }

        String dtime = hos::GetCurrentTime();
        u32 blv = hos::GetBatteryLevel();
        bool isch = hos::IsCharging();
        if((this->preblv != blv) || this->vfirst)
        {
            if(blv <= 10) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/0.png"));
            else if((blv > 10) && (blv <= 20)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/10.png"));
            else if((blv > 20) && (blv <= 30)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/20.png"));
            else if((blv > 30) && (blv <= 40)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/30.png"));
            else if((blv > 40) && (blv <= 50)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/40.png"));
            else if((blv > 50) && (blv <= 60)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/50.png"));
            else if((blv > 60) && (blv <= 70)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/60.png"));
            else if((blv > 70) && (blv <= 80)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/70.png"));
            else if((blv > 80) && (blv <= 90)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/80.png"));
            else if((blv > 90) && (blv < 100)) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/90.png"));
            else if(blv == 100) this->batteryImage->SetImage(global_settings.PathForResource("/Battery/100.png"));
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
        this->hasusb = usb::detail::IsStateOk();
        this->usbImage->SetVisible(this->hasusb);
        u32 connstr = 0;
        Result rc = nifmGetInternetConnectionStatus(NULL, &connstr, NULL);
        std::string connimg = "None";
        if(rc == 0) if(connstr > 0) connimg = std::to_string(connstr);
        if(connstr != this->connstate)
        {
            this->connImage->SetImage(global_settings.PathForResource("/Connection/" + connimg + ".png"));
            this->connImage->SetWidth(40);
            this->connImage->SetHeight(40);
            this->connstate = connstr;
        }
        if(connstr > 0)
        {
            u32 ip = gethostid();
            char sip[0x20] = {0};
            inet_ntop(AF_INET, &ip, sip, 0x20);
            this->ipText->SetText(String(sip));
        }
        else this->ipText->SetText("");
        auto user = acc::GetSelectedUser();
        if(!acc::UidCompare(&user, &this->seluser))
        {
            this->seluser = user;
            if(!accountUidIsValid(&this->seluser)) this->userImage->SetImage(global_settings.PathForResource("/Common/User.png"));
            else
            {
                auto usericon = acc::GetCachedUserIcon();
                if(fs::Exists(usericon)) this->userImage->SetImage(usericon);
                else this->userImage->SetImage(global_settings.PathForResource("/Common/User.png"));
            }
            this->userImage->SetWidth(70);
            this->userImage->SetHeight(70);
        }
    }

    void MainApplication::LoadMenuData(String Name, std::string ImageName, String TempHead, bool CommonIcon)
    {
        if(this->menuImage != NULL)
        {
            this->menuImage->SetVisible(true);
            if(CommonIcon) this->menuImage->SetImage(global_settings.PathForResource("/Common/" + ImageName + ".png"));
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

    void MainApplication::LoadMenuHead(String Head)
    {
        if(this->menuHeadText != NULL) this->menuHeadText->SetText(Head);
    }

    void MainApplication::UnloadMenuData()
    {
        this->menuImage->SetVisible(false);
        this->menuNameText->SetVisible(false);
        this->menuHeadText->SetVisible(false);
    }

    void MainApplication::browser_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            if(this->browser->GoBack()) this->browser->UpdateElements(-1);
            else
            {
                this->UnloadMenuData();
                this->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
                this->LoadLayout(this->exploreMenu);
            }
        }
        else if(down & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = this->browser->GetExplorer()->IsDirectory(clipboard);
                std::string fsicon;
                if(cdir) fsicon = global_settings.PathForResource("/FileSystem/Directory.png");
                else
                {
                    String ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") fsicon = global_settings.PathForResource("/FileSystem/NSP.png");
                    else if(ext == "nro") fsicon = global_settings.PathForResource("/FileSystem/NRO.png");
                    else if(ext == "tik") fsicon = global_settings.PathForResource("/FileSystem/TIK.png");
                    else if(ext == "cert") fsicon = global_settings.PathForResource("/FileSystem/CERT.png");
                    else if(ext == "nca") fsicon = global_settings.PathForResource("/FileSystem/NCA.png");
                    else if(ext == "nxtheme") fsicon = global_settings.PathForResource("/FileSystem/NXTheme.png");
                    else fsicon = global_settings.PathForResource("/FileSystem/File.png");
                }
                int sopt = this->CreateShowDialog(set::GetDictionaryEntry(222), set::GetDictionaryEntry(223) + "\n(" + clipboard + ")", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true, fsicon);
                if(sopt == 0)
                {
                    String cname = fs::GetFileName(clipboard);
                    this->LoadLayout(this->GetCopyLayout());
                    this->GetCopyLayout()->StartCopy(clipboard, this->browser->GetExplorer()->FullPathFor(cname), cdir, this->browser->GetExplorer());
                    global_app->LoadLayout(this->browser);
                    this->browser->UpdateElements();
                    clipboard = "";
                }
            }
            else this->ShowNotification(set::GetDictionaryEntry(224));
        }
        else if(down & KEY_L)
        {
            String cfile = AskForText(set::GetDictionaryEntry(225), "");
            if(cfile != "")
            {
                String ffile = this->browser->GetExplorer()->FullPathFor(cfile);
                String pffile = this->browser->GetExplorer()->FullPresentablePathFor(cfile);
                if(this->browser->GetExplorer()->IsFile(ffile) || this->browser->GetExplorer()->IsDirectory(ffile)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(255));
                else
                {
                    this->browser->GetExplorer()->CreateFile(ffile);
                    this->ShowNotification(set::GetDictionaryEntry(227) + " \'" + pffile + "\'");
                    this->browser->UpdateElements();
                }
            }
        }
        else if(down & KEY_R)
        {
            String cdir = AskForText(set::GetDictionaryEntry(250), "");
            if(cdir != "")
            {
                String fdir = this->browser->GetExplorer()->FullPathFor(cdir);
                String pfdir = this->browser->GetExplorer()->FullPresentablePathFor(cdir);
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

    void MainApplication::exploreMenu_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::pcExplore_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
            this->LoadLayout(this->exploreMenu);
        }
    }

    void MainApplication::fileContent_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B) this->LoadLayout(this->browser);
        else if((down & KEY_DDOWN) || (down & KEY_LSTICK_DOWN) || (held & KEY_RSTICK_DOWN)) this->fileContent->ScrollDown();
        else if((down & KEY_DUP) || (down & KEY_LSTICK_UP) || (held & KEY_RSTICK_UP)) this->fileContent->ScrollUp();
    }

    void MainApplication::contentInformation_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(189));
            this->LoadLayout(this->storageContents);
        }
    }

    void MainApplication::storageContents_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(33));
            this->LoadLayout(this->contentManager);
        }
    }

    void MainApplication::contentManager_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::unusedTickets_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::account_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::amiibo_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::settings_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::memory_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
            this->LoadLayout(this->settings);
        }
    }

    void MainApplication::webBrowser_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::about_Input(u64 down, u64 up, u64 held)
    {
        if(down & KEY_B)
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

    void MainApplication::OnInput(u64 down, u64 up, u64 held)
    {
        if(down & KEY_MINUS) this->CloseWithFadeOut();
        else if((down & KEY_ZL) || (down & KEY_ZR)) ShowPowerTasksDialog(set::GetDictionaryEntry(229), set::GetDictionaryEntry(230));
        else if(down & KEY_PLUS) this->helpImage_OnClick();
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

    SettingsLayout::Ref &MainApplication::GetSettingsLayout()
    {
        return this->settings;
    }

    MemoryLayout::Ref &MainApplication::GetMemoryLayout()
    {
        return this->memory;
    }

    UpdateLayout::Ref &MainApplication::GetUpdateLayout()
    {
        return this->update;
    }

    WebBrowserLayout::Ref &MainApplication::GetWebBrowserLayout()
    {
        return this->webBrowser;
    }

    AboutLayout::Ref &MainApplication::GetAboutLayout()
    {
        return this->about;
    }

    void UpdateClipboard(String Path)
    {
        SetClipboard(Path);
        String copymsg = set::GetDictionaryEntry(258);
        if(global_app->GetBrowserLayout()->GetExplorer()->IsFile(Path)) copymsg = set::GetDictionaryEntry(257);
        global_app->ShowNotification(copymsg);
    }
}