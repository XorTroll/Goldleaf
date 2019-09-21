
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

#include <ui/ui_USBDrivesLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    USBDrivesLayout::USBDrivesLayout() : pu::ui::Layout()
    {
        this->drivesMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->drivesMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->drivesMenu);
        this->noDrivesText = pu::ui::elm::TextBlock::New(30, 630, "No drives were found.");
        this->noDrivesText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->noDrivesText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->noDrivesText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->noDrivesText);
        this->Add(this->drivesMenu);
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
                pu::ui::elm::MenuItem *mitm = pu::ui::elm::MenuItem(slabel);
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