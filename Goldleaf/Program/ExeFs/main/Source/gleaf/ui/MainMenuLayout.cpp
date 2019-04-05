#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    MainMenuLayout::MainMenuLayout() : pu::Layout()
    {
        this->optionMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->optionMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->optionMenu->SetOnSelectionChanged(std::bind(&MainMenuLayout::optionMenu_SelectionChanged, this));
        this->sdcardMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(0));
        this->sdcardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdcardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdcardMenuItem->AddOnClick(std::bind(&MainMenuLayout::sdcardMenuItem_Click, this));
        this->nandMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(1));
        this->nandMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandMenuItem->AddOnClick(std::bind(&MainMenuLayout::nandMenuItem_Click, this));
        this->pcdriveMenuItem = new pu::element::MenuItem("Browse PC drive (via USB)");
        this->pcdriveMenuItem->SetIcon(gsets.PathForResource("/Common/Drive.png"));
        this->pcdriveMenuItem->SetColor(gsets.CustomScheme.Text);
        this->pcdriveMenuItem->AddOnClick(std::bind(&MainMenuLayout::pcdriveMenuItem_Click, this));
        this->usbMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(2));
        this->usbMenuItem->SetIcon(gsets.PathForResource("/Common/USB.png"));
        this->usbMenuItem->SetColor(gsets.CustomScheme.Text);
        this->usbMenuItem->AddOnClick(std::bind(&MainMenuLayout::usbMenuItem_Click, this));
        this->titleMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(3));
        this->titleMenuItem->SetIcon(gsets.PathForResource("/Common/Storage.png"));
        this->titleMenuItem->SetColor(gsets.CustomScheme.Text);
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->ticketMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(4));
        this->ticketMenuItem->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
        this->ticketMenuItem->SetColor(gsets.CustomScheme.Text);
        this->ticketMenuItem->AddOnClick(std::bind(&MainMenuLayout::ticketMenuItem_Click, this));
        this->webMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(5));
        this->webMenuItem->SetIcon(gsets.PathForResource("/Common/Browser.png"));
        this->webMenuItem->SetColor(gsets.CustomScheme.Text);
        this->webMenuItem->AddOnClick(std::bind(&MainMenuLayout::webMenuItem_Click, this));
        this->accountMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(6));
        this->accountMenuItem->SetIcon(gsets.PathForResource("/Common/Accounts.png"));
        this->accountMenuItem->SetColor(gsets.CustomScheme.Text);
        this->accountMenuItem->AddOnClick(std::bind(&MainMenuLayout::accountMenuItem_Click, this));
        this->sysinfoMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(7));
        this->sysinfoMenuItem->SetIcon(gsets.PathForResource("/Common/Settings.png"));
        this->sysinfoMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sysinfoMenuItem->AddOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->updateMenuItem = new pu::element::MenuItem("Update");
        this->updateMenuItem->SetIcon(gsets.PathForResource("/Common/Update.png"));
        this->updateMenuItem->SetColor(gsets.CustomScheme.Text);
        this->updateMenuItem->AddOnClick(std::bind(&MainMenuLayout::updateMenuItem_Click, this));
        this->aboutMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(8));
        this->aboutMenuItem->SetIcon(gsets.PathForResource("/Common/Info.png"));
        this->aboutMenuItem->SetColor(gsets.CustomScheme.Text);
        this->aboutMenuItem->AddOnClick(std::bind(&MainMenuLayout::aboutMenuItem_Click, this));
        this->optionMenu->AddItem(this->sdcardMenuItem);
        this->optionMenu->AddItem(this->nandMenuItem);
        this->optionMenu->AddItem(this->pcdriveMenuItem);
        this->optionMenu->AddItem(this->usbMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->ticketMenuItem);
        this->optionMenu->AddItem(this->webMenuItem);
        this->optionMenu->AddItem(this->accountMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->updateMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->AddChild(this->optionMenu);
    }

    void MainMenuLayout::optionMenu_SelectionChanged()
    {
        std::string info;
        pu::element::MenuItem *isel = this->optionMenu->GetSelectedItem();
        if(isel == this->sdcardMenuItem) info = set::GetDictionaryEntry(9);
        else if(isel == this->nandMenuItem) info = set::GetDictionaryEntry(10);
        else if(isel == this->pcdriveMenuItem) info = "Browse a drive from a remote PC (like C:\\) via USB";
        else if(isel == this->usbMenuItem) info = set::GetDictionaryEntry(11);
        else if(isel == this->titleMenuItem) info = set::GetDictionaryEntry(12);
        else if(isel == this->ticketMenuItem) info = set::GetDictionaryEntry(13);
        else if(isel == this->webMenuItem) info = set::GetDictionaryEntry(14);
        else if(isel == this->accountMenuItem) info = set::GetDictionaryEntry(15);
        else if(isel == this->sysinfoMenuItem) info = set::GetDictionaryEntry(16);
        else if(isel == this->updateMenuItem) info = "Check for Goldleaf updates";
        else if(isel == this->aboutMenuItem) info = set::GetDictionaryEntry(17);
        mainapp->UpdateFooter(info);
    }

    void MainMenuLayout::sdcardMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(19), "SdCard", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->GetBrowserLayout()->ChangePartitionSdCard();
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void MainMenuLayout::nandMenuItem_Click()
    {
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(20), set::GetDictionaryEntry(21) + "\n\n" + set::GetDictionaryEntry(22) + "\n" + set::GetDictionaryEntry(23) + "\n" + set::GetDictionaryEntry(24) + "\n" + set::GetDictionaryEntry(25), { set::GetDictionaryEntry(26), set::GetDictionaryEntry(27), set::GetDictionaryEntry(28), set::GetDictionaryEntry(29), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        if(sopt == 0) mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        else if(sopt == 1) mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        else if(sopt == 2) mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        else if(sopt == 3) mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void MainMenuLayout::pcdriveMenuItem_Click()
    {
        if(usb::IsStateNotReady())
        {
            mainapp->CreateShowDialog("PC drive browser", "USB isn't connected to a PC. Please connect it to a PC.", { "Ok" }, true);
            return;
        }
        if(usb::WriteCommandInput(usb::NewCommandId::ListSystemDrives))
        {
            u32 drivecount = usb::Read32();
            std::vector<std::string> drivenames;
            std::vector<std::string> drivemounts;
            std::vector<std::string> opts;
            for(u32 i = 0; i < drivecount; i++)
            {
                std::string name = usb::ReadString();
                std::string mount = usb::ReadString();
                drivenames.push_back(name);
                drivemounts.push_back(mount);
                opts.push_back(mount + ":\\");
            }
            opts.push_back("Cancel");
            int sopt = mainapp->CreateShowDialog("PC drive browser", "Which drive would you like to browse?", opts, true);
            if(sopt < 0) return;
            mainapp->GetBrowserLayout()->ChangePartitionPCDrive(drivemounts[sopt]);
            mainapp->LoadMenuData("PC drive browser", "Drive", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
            mainapp->LoadLayout(mainapp->GetBrowserLayout());
        }
    }

    void MainMenuLayout::usbMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(30), "USB", set::GetDictionaryEntry(31));
        mainapp->LoadLayout(mainapp->GetUSBInstallLayout());
        mainapp->GetUSBInstallLayout()->StartUSBConnection();
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(32), "Storage", set::GetDictionaryEntry(33));
        EnsureDirectories();
        mainapp->LoadLayout(mainapp->GetContentManagerLayout());
    }

    void MainMenuLayout::ticketMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(34), "Ticket", set::GetDictionaryEntry(35));
        mainapp->GetTicketManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTicketManagerLayout());
    }

    void MainMenuLayout::webMenuItem_Click()
    {
        if(!IsInstalledTitle()) mainapp->CreateShowDialog(set::GetDictionaryEntry(36), set::GetDictionaryEntry(37), { set::GetDictionaryEntry(234) }, false);
        std::string out = AskForText(set::GetDictionaryEntry(38), "https://");
        if(out == "") return;
        else
        {
            bool nothttp = (out.substr(0, 6) != "http:/");
            bool nothttps = (out.substr(0, 7) != "https:/");
            if(nothttp && nothttps)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(36), set::GetDictionaryEntry(39), { set::GetDictionaryEntry(234) }, false);
                return;
            }
        }
        if(IsInstalledTitle())
        {
            WebCommonConfig web;
            webPageCreate(&web, out.c_str());
            WebCommonReply wout;
            webConfigShow(&web, &wout);  
        }
        else
        {
            WebWifiConfig wwf;
            webWifiCreate(&wwf, NULL, out.c_str(), 0, 0);
            WebWifiReturnValue vret;
            Result rc = webWifiShow(&wwf, &vret);
            if(rc != 0) HandleResult(rc, set::GetDictionaryEntry(40));
        }
    }

    void MainMenuLayout::accountMenuItem_Click()
    {
        u128 uid = AskForUser();
        if(uid == 0) return;
        mainapp->LoadMenuData(set::GetDictionaryEntry(41), "Accounts", set::GetDictionaryEntry(42));
        mainapp->GetAccountLayout()->Load(uid);
        mainapp->LoadLayout(mainapp->GetAccountLayout());
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    void MainMenuLayout::updateMenuItem_Click()
    {
        mainapp->LoadMenuData("Update", "Update", "Update Goldleaf");
        mainapp->LoadLayout(mainapp->GetUpdateLayout());
        mainapp->GetUpdateLayout()->StartUpdateSearch();
    }

    void MainMenuLayout::aboutMenuItem_Click()
    {
        std::string rmode = set::GetDictionaryEntry(45);
        if(IsNRO()) rmode = set::GetDictionaryEntry(46);
        else if(IsInstalledTitle()) rmode = set::GetDictionaryEntry(47);
        else if(IsQlaunch()) rmode = set::GetDictionaryEntry(78);
        mainapp->LoadMenuData("Goldleaf v" + GetVersion(), "Info", rmode);
        mainapp->LoadLayout(mainapp->GetAboutLayout());
    }
}