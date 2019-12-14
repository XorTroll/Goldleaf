
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

extern ui::MainApplication::Ref mainapp;
extern set::Settings gsets;

namespace ui
{
    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout()
    {
        this->mountsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->mountsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->mountsMenu);
        this->sdCardMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(19));
        this->sdCardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::sdCard_Click, this));
        this->pcDriveMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(279));
        this->pcDriveMenuItem->SetIcon(gsets.PathForResource("/Common/Drive.png"));
        this->pcDriveMenuItem->SetColor(gsets.CustomScheme.Text);
        this->pcDriveMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::pcDrive_Click, this));
        this->nandProfInfoFMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(20) + " (PRODINFOF)");
        this->nandProfInfoFMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandProfInfoFMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandProfInfoFMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandProdInfoF_Click, this));
        this->nandSafeMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(20) + " (SAFE)");
        this->nandSafeMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSafeMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSafeMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandSafe_Click, this));
        this->nandUserMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(20) + " (USER)");
        this->nandUserMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ExploreMenuLayout::nandUser_Click, this));
        this->nandSystemMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(20) + " (SYSTEM)");
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

    void ExploreMenuLayout::sdCard_Click()
    {
        mainapp->GetBrowserLayout()->ChangePartitionSdCard();
        mainapp->LoadMenuData(set::GetDictionaryEntry(19), "SdCard", mainapp->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void ExploreMenuLayout::pcDrive_Click()
    {
        if(!usb::detail::IsStateOk())
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