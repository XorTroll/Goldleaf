#include <ui/ui_USBDrivesLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    USBDrivesLayout::USBDrivesLayout() : pu::ui::Layout()
    {
        this->drivesMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->drivesMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->drivesMenu);
        this->noDrivesText = new pu::ui::elm::TextBlock(30, 630, "No drives were found.");
        this->noDrivesText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->noDrivesText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->noDrivesText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->noDrivesText);
        this->Add(this->drivesMenu);
    }

    USBDrivesLayout::~USBDrivesLayout()
    {
        delete this->drivesMenu;
    }

    void USBDrivesLayout::Start()
    {
        /*
        if(!this->drvs.empty()) for(u32 i = 0; i < this->drvs.size(); i++)
        {
            drive::DriveClose(this->drvs[i]);
        }
        this->drivesMenu->ClearItems();
        this->drvs.clear();
        drive::Exit();
        drive::Initialize();
        s32 count = 0;
        drive::WaitForDrives(1000000000L);
        drive::CountDrives(&count);
        if(count > 0)
        {
            this->drivesMenu->SetVisible(true);
            this->noDrivesText->SetVisible(false);
            for(int i = 0; i < count; i++)
            {
                drive::Drive *drv = (drive::Drive*)malloc(sizeof(drive::Drive));
                memset(drv, 0, sizeof(drive::Drive));
                drive::OpenDrive(i, drv);
                this->drvs.push_back(drv);
            }
        }
        else
        {
            this->drivesMenu->SetVisible(false);
            this->noDrivesText->SetVisible(true);
        }
        UpdateDrives();
        */
    }

    void USBDrivesLayout::UpdateDrives()
    {
        /*
        if(!drvs.empty())
        {
            this->drivesMenu->SetCooldownEnabled(true);
            for(u32 i = 0; i < this->drvs.size(); i++)
            {
                auto drv = this->drvs[i];
                drive::DriveMount(drv, "0");
                char label[256];
                drive::f_getlabel("0:", label, NULL);
                drive::DriveUnmount(drv);
                pu::String slabel;
                if(strlen(label) == 0) slabel = "Drive " + std::to_string(i);
                else slabel = pu::String(label);
                pu::ui::elm::MenuItem *mitm = new pu::ui::elm::MenuItem(slabel);
                mitm->SetIcon(gsets.PathForResource("/Common/Drive.png"));
                mitm->AddOnClick(std::bind(&USBDrivesLayout::drive_Click, this));
                this->drivesMenu->AddItem(mitm);
            }
        }
        */
    }

    void USBDrivesLayout::drive_Click()
    {
        /*
        auto drv = this->drvs[this->drivesMenu->GetSelectedIndex()];
        mainapp->GetBrowserLayout()->ChangePartitionUSBDrive(drv);
        mainapp->LoadMenuData("USB drive explorer", "USB", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
        */
    }
}