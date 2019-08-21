#include <ui/ui_ExploreMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout()
    {
        this->mountsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->mountsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->mountsMenu);
        this->sdCardMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(19));
        this->sdCardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::sdCard_Click, this));
        this->pcDriveMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(279));
        this->pcDriveMenuItem->SetIcon(gsets.PathForResource("/Common/Drive.png"));
        this->pcDriveMenuItem->SetColor(gsets.CustomScheme.Text);
        this->pcDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::pcDrive_Click, this));
        this->nandProfInfoFMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(20) + " (PRODINFOF)");
        this->nandProfInfoFMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandProfInfoFMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandProfInfoFMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandProdInfoF_Click, this));
        this->nandSafeMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(20) + " (SAFE)");
        this->nandSafeMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSafeMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSafeMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSafe_Click, this));
        this->nandUserMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(20) + " (USER)");
        this->nandUserMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandUser_Click, this));
        this->nandSystemMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(20) + " (SYSTEM)");
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
            mainapp->CreateShowDialog(set::GetDictionaryEntry(299), set::GetDictionaryEntry(300), { set::GetDictionaryEntry(234) }, true);
            return;
        }
        mainapp->GetPCExploreLayout()->UpdatePaths();
        mainapp->LoadLayout(mainapp->GetPCExploreLayout());
    }

    void ExploreMenuLayout::nandProdInfoF_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        mainapp->LoadMenuData(set::GetDictionaryEntry(1), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSafe_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        mainapp->LoadMenuData(set::GetDictionaryEntry(1), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandUser_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        mainapp->LoadMenuData(set::GetDictionaryEntry(1), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSystem_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        mainapp->LoadMenuData(set::GetDictionaryEntry(1), "NAND", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }
}