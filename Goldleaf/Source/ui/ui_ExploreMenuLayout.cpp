
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <ui/ui_ExploreMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern cfg::Settings global_settings;

namespace ui
{
    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout()
    {
        this->mountsMenu = pu::ui::elm::Menu::New(0, 160, 1280, global_settings.custom_scheme.Base, global_settings.menu_item_size, (560 / global_settings.menu_item_size));
        this->mountsMenu->SetOnFocusColor(global_settings.custom_scheme.BaseFocus);
        global_settings.ApplyScrollBarColor(this->mountsMenu);
        this->UpdateMenu();
        this->Add(this->mountsMenu);
    }

    ExploreMenuLayout::~ExploreMenuLayout()
    {
        for(auto &exp: this->mountedExplorers)
        {
            delete exp;
        }
    }

    void ExploreMenuLayout::UpdateMenu()
    {
        this->mountsMenu->ClearItems();
        this->sdCardMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(19));
        this->sdCardMenuItem->SetIcon(global_settings.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::sdCard_Click, this));
        this->pcDriveMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(279));
        this->pcDriveMenuItem->SetIcon(global_settings.PathForResource("/Common/Drive.png"));
        this->pcDriveMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->pcDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::pcDrive_Click, this));
        this->usbDriveMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(398));
        this->usbDriveMenuItem->SetIcon(global_settings.PathForResource("/Common/Drive.png"));
        this->usbDriveMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->usbDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::usbDrive_Click, this));
        this->nandProfInfoFMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (PRODINFOF)");
        this->nandProfInfoFMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandProfInfoFMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandProfInfoFMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandProdInfoF_Click, this));
        this->nandSafeMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (SAFE)");
        this->nandSafeMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandSafeMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandSafeMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSafe_Click, this));
        this->nandUserMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (USER)");
        this->nandUserMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandUser_Click, this));
        this->nandSystemMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (SYSTEM)");
        this->nandSystemMenuItem->SetIcon(global_settings.PathForResource("/Common/NAND.png"));
        this->nandSystemMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->nandSystemMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSystem_Click, this));
        this->mountsMenu->AddItem(this->sdCardMenuItem);
        this->mountsMenu->AddItem(this->pcDriveMenuItem);
        this->mountsMenu->AddItem(this->usbDriveMenuItem);
        this->mountsMenu->AddItem(this->nandProfInfoFMenuItem);
        this->mountsMenu->AddItem(this->nandSafeMenuItem);
        this->mountsMenu->AddItem(this->nandUserMenuItem);
        this->mountsMenu->AddItem(this->nandSystemMenuItem);
        for(auto &mount: this->mounts)
        {
            this->mountsMenu->AddItem(mount);
        }
        this->mountsMenu->SetSelectedIndex(0);
    }

    void ExploreMenuLayout::sdCard_Click()
    {
        global_app->GetBrowserLayout()->ChangePartitionSdCard();
        global_app->LoadMenuData(cfg::strings::Main.GetString(19), "SdCard", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::pcDrive_Click()
    {
        if(!usb::detail::IsStateOk())
        {
            global_app->CreateShowDialog(cfg::strings::Main.GetString(299), cfg::strings::Main.GetString(300), { cfg::strings::Main.GetString(234) }, true);
            return;
        }
        global_app->GetPCExploreLayout()->UpdatePaths();
        global_app->LoadLayout(global_app->GetPCExploreLayout());
    }

    void ExploreMenuLayout::usbDrive_Click()
    {
        if(!drive::IsFspUsbAccessible())
        {
            global_app->CreateShowDialog(cfg::strings::Main.GetString(399), cfg::strings::Main.GetString(400), { cfg::strings::Main.GetString(234) }, false);
            return;
        }

        auto drives = drive::ListDrives();
        std::vector<String> opts;
        for(auto &drive: drives) opts.push_back(drive.label);
        opts.push_back(cfg::strings::Main.GetString(18));
        int sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(401), cfg::strings::Main.GetString(402) + " " + std::to_string(drives.size()), opts, true);
        if(!drives.empty())
        {
            if(sopt < drives.size())
            {
                auto drv = drives[sopt];
                global_app->GetBrowserLayout()->ChangePartitionUSBDrive(drv);
                global_app->LoadMenuData(cfg::strings::Main.GetString(403), "Drive", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
                global_app->LoadLayout(global_app->GetBrowserLayout());
            }
        }
    }

    void ExploreMenuLayout::nandProdInfoF_Click()
    {
        global_app->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        global_app->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSafe_Click()
    {
        global_app->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        global_app->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandUser_Click()
    {
        global_app->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        global_app->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSystem_Click()
    {
        global_app->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        global_app->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_Click(fs::Explorer *exp, String name, std::string icon)
    {
        global_app->GetBrowserLayout()->ChangePartitionExplorer(exp);
        global_app->LoadMenuData(name, icon, global_app->GetBrowserLayout()->GetExplorer()->GetPresentableCwd(), false);
        global_app->LoadLayout(global_app->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_Click_X(fs::Explorer *exp)
    {
        auto sopt = global_app->CreateShowDialog(cfg::strings::Main.GetString(404), cfg::strings::Main.GetString(405), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(sopt == 0)
        {
            u32 idx = 0;
            bool found = false;
            for(auto &mounted: this->mountedExplorers)
            {
                if(exp == mounted)
                {
                    found = true;
                    break;
                }
                idx++;
            }
            if(found)
            {
                delete exp;
                this->mountedExplorers.erase(this->mountedExplorers.begin() + idx);
                this->mounts.erase(this->mounts.begin() + idx);
                this->UpdateMenu();
                global_app->ShowNotification(cfg::strings::Main.GetString(406));
            }
        }
    }

    void ExploreMenuLayout::AddMountedExplorer(fs::Explorer *exp, String name, std::string icon)
    {
        auto itm = pu::ui::elm::MenuItem::New(name);
        itm->SetIcon(icon);
        itm->SetColor(global_settings.custom_scheme.Text);
        itm->AddOnClick(std::bind(&ExploreMenuLayout::explorer_Click, this, exp, name, icon));
        itm->AddOnClick(std::bind(&ExploreMenuLayout::explorer_Click_X, this, exp), KEY_X);
        this->mounts.push_back(itm);
        this->mountedExplorers.push_back(exp);
    }

    std::vector<fs::Explorer*> &ExploreMenuLayout::GetMountedExplorers()
    {
        return this->mountedExplorers;
    }
}