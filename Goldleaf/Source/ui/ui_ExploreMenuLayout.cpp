
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
        this->sdCardMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(19));
        this->sdCardMenuItem->SetIcon(global_settings.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::sdCard_Click, this));
        this->pcDriveMenuItem = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(279));
        this->pcDriveMenuItem->SetIcon(global_settings.PathForResource("/Common/Drive.png"));
        this->pcDriveMenuItem->SetColor(global_settings.custom_scheme.Text);
        this->pcDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::pcDrive_Click, this));
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
        this->mountsMenu->AddItem(this->nandProfInfoFMenuItem);
        this->mountsMenu->AddItem(this->nandSafeMenuItem);
        this->mountsMenu->AddItem(this->nandUserMenuItem);
        this->mountsMenu->AddItem(this->nandSystemMenuItem);
        this->Add(this->mountsMenu);
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
}