#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    ExploreMenuLayout::ExploreMenuLayout() : pu::Layout()
    {
        this->mountsMenu = new pu::element::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->mountsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->mountsMenu->SetOnSelectionChanged(std::bind(&ExploreMenuLayout::mountsMenu_SelectionChanged, this));
        this->sdCardMenuItem = new pu::element::MenuItem("SD card");
        this->sdCardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::sdCard_Click, this));
        this->pcDriveMenuItem = new pu::element::MenuItem("PC drive (via USB)");
        this->pcDriveMenuItem->SetIcon(gsets.PathForResource("/Common/Drive.png"));
        this->pcDriveMenuItem->SetColor(gsets.CustomScheme.Text);
        this->pcDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::pcDrive_Click, this));
        this->nandProfInfoFMenuItem = new pu::element::MenuItem("Console memory (PRODINFOF)");
        this->nandProfInfoFMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandProfInfoFMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandProfInfoFMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandProdInfoF_Click, this));
        this->nandSafeMenuItem = new pu::element::MenuItem("Console memory (SAFE)");
        this->nandSafeMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSafeMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSafeMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSafe_Click, this));
        this->nandUserMenuItem = new pu::element::MenuItem("Console memory (USER)");
        this->nandUserMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandUser_Click, this));
        this->nandSystemMenuItem = new pu::element::MenuItem("Console memory (SYSTEM)");
        this->nandSystemMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSystemMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSystemMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSystem_Click, this));
        this->mountsMenu->AddItem(this->sdCardMenuItem);
        this->mountsMenu->AddItem(this->pcDriveMenuItem);
        this->mountsMenu->AddItem(this->nandProfInfoFMenuItem);
        this->mountsMenu->AddItem(this->nandSafeMenuItem);
        this->mountsMenu->AddItem(this->nandUserMenuItem);
        this->mountsMenu->AddItem(this->nandSystemMenuItem);
        this->Add(this->mountsMenu);
    }

    ExploreMenuLayout::~ExploreMenuLayout()
    {
        delete this->mountsMenu;
    }

    void ExploreMenuLayout::mountsMenu_SelectionChanged()
    {
        
    }

    void ExploreMenuLayout::sdCard_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionSdCard();
        mainapp->LoadMenuData(set::GetDictionaryEntry(19), "SdCard", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::pcDrive_Click()
    {
        if(usb::IsStateNotReady())
        {
            mainapp->CreateShowDialog("PC drive browser", "USB isn't connected to a PC. Please connect it to a PC.", { "Ok" }, true);
            return;
        }
        if(usb::WriteCommandInput(usb::CommandId::ListSystemDrives))
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

    void ExploreMenuLayout::nandProdInfoF_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSafe_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandUser_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSystem_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::otherMount_Click()
    {
        
    }

    void ExploreMenuLayout::specialMount_Click_X()
    {
        mainapp->CreateShowDialog("Unmount explorer", "This partition is special so cannot be unmounted.", { "Ok" }, true);
    }

    void ExploreMenuLayout::otherMount_Click_X()
    {
        
    }
}