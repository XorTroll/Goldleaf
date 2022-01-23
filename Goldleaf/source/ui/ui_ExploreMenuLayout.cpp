
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout() {
        this->mounts_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->mounts_menu);
        this->UpdateMenu();
        this->Add(this->mounts_menu);
    }

    ExploreMenuLayout::~ExploreMenuLayout() {
        for(auto &mounted_exp: this->mounted_explorers) {
            delete mounted_exp;
        }
    }

    void ExploreMenuLayout::UpdateMenu() {
        this->mounts_menu->ClearItems();
        this->sd_card_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(19));
        this->sd_card_menu_item->SetIcon(g_Settings.PathForResource("/Common/SdCard.png"));
        this->sd_card_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->sd_card_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::sdCard_DefaultKey, this));
        this->remote_pc_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(279));
        this->remote_pc_menu_item->SetIcon(g_Settings.PathForResource("/Common/Drive.png"));
        this->remote_pc_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->remote_pc_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::pcDrive_DefaultKey, this));
        this->usb_drive_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(398));
        this->usb_drive_menu_item->SetIcon(g_Settings.PathForResource("/Common/Drive.png"));
        this->usb_drive_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->usb_drive_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::usbDrive_DefaultKey, this));
        this->nand_prodinfof_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (PRODINFOF)");
        this->nand_prodinfof_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_prodinfof_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_prodinfof_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandProdInfoF_DefaultKey, this));
        this->nand_safe_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (SAFE)");
        this->nand_safe_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_safe_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_safe_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandSafe_DefaultKey, this));
        this->nand_user_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (USER)");
        this->nand_user_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_user_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_user_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandUser_DefaultKey, this));
        this->nand_system_menu_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(20) + " (SYSTEM)");
        this->nand_system_menu_item->SetIcon(g_Settings.PathForResource("/Common/NAND.png"));
        this->nand_system_menu_item->SetColor(g_Settings.custom_scheme.text);
        this->nand_system_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandSystem_DefaultKey, this));
        this->mounts_menu->AddItem(this->sd_card_menu_item);
        this->mounts_menu->AddItem(this->remote_pc_menu_item);
        this->mounts_menu->AddItem(this->usb_drive_menu_item);
        this->mounts_menu->AddItem(this->nand_prodinfof_menu_item);
        this->mounts_menu->AddItem(this->nand_safe_menu_item);
        this->mounts_menu->AddItem(this->nand_user_menu_item);
        this->mounts_menu->AddItem(this->nand_system_menu_item);
        for(auto &mount: this->mounted_explorer_items) {
            this->mounts_menu->AddItem(mount);
        }
        this->mounts_menu->SetSelectedIndex(0);
    }

    void ExploreMenuLayout::sdCard_DefaultKey() {
        g_MainApplication->GetBrowserLayout()->ChangePartitionSdCard();
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(19), "SdCard", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::pcDrive_DefaultKey() {
        if(usb::detail::IsStateOk()) {
            g_MainApplication->GetPCExploreLayout()->UpdatePaths();
            g_MainApplication->LoadLayout(g_MainApplication->GetPCExploreLayout());
        }
        else {
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(299), cfg::strings::Main.GetString(300), { cfg::strings::Main.GetString(234) }, true);
        }
    }

    void ExploreMenuLayout::usbDrive_DefaultKey() {
        auto drives = drive::ListDrives();
        std::vector<std::string> options;
        for(const auto &drive: drives) {
            options.push_back(drive.name);
        }
        options.push_back(cfg::strings::Main.GetString(18));
        const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(401), cfg::strings::Main.GetString(402) + " " + std::to_string(drives.size()), options, true);
        if(!drives.empty()) {
            if(option_1 < drives.size()) {
                auto &drive = drives[option_1];
                const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(401), cfg::strings::Main.GetString(434), { cfg::strings::Main.GetString(136), cfg::strings::Main.GetString(435), cfg::strings::Main.GetString(18) }, true);
                if(option_2 == 0) {
                    g_MainApplication->GetBrowserLayout()->ChangePartitionDrive(drive);
                    g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(403), "Drive", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
                    g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
                }
                else if(option_2 == 1) {
                    if(drive::UnmountDrive(drive)) {
                        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(436));
                    }
                    else {
                        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(437));
                    }
                }
            }
        }
    }

    void ExploreMenuLayout::nandProdInfoF_DefaultKey() {
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSafe_DefaultKey() {
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandUser_DefaultKey() {
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSystem_DefaultKey() {
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(1), "NAND", g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_DefaultKey(fs::Explorer *exp, const std::string &name, const std::string &icon) {
        g_MainApplication->GetBrowserLayout()->ChangePartitionExplorer(exp);
        g_MainApplication->LoadMenuData(name, icon, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd(), false);
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_X(fs::Explorer *exp) {
        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(404), cfg::strings::Main.GetString(405), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        if(option == 0) {
            u32 mounted_exp_idx = 0;
            auto found = false;
            for(auto &mounted: this->mounted_explorers) {
                if(exp == mounted) {
                    found = true;
                    break;
                }
                mounted_exp_idx++;
            }
            if(found) {
                delete exp;
                this->mounted_explorers.erase(this->mounted_explorers.begin() + mounted_exp_idx);
                this->mounted_explorer_items.erase(this->mounted_explorer_items.begin() + mounted_exp_idx);
                this->UpdateMenu();
                g_MainApplication->ShowNotification(cfg::strings::Main.GetString(406));
            }
            else {
                // TODO (low priority): show anything if not found?
            }
        }
    }

    void ExploreMenuLayout::AddMountedExplorer(fs::Explorer *exp, const std::string &name, const std::string &icon) {
        auto mounted_exp_item = pu::ui::elm::MenuItem::New(name);
        mounted_exp_item->SetIcon(icon);
        mounted_exp_item->SetColor(g_Settings.custom_scheme.text);
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::explorer_DefaultKey, this, exp, name, icon));
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::explorer_X, this, exp), HidNpadButton_X);
        this->mounted_explorer_items.push_back(mounted_exp_item);
        this->mounted_explorers.push_back(exp);
    }

    std::vector<fs::Explorer*> &ExploreMenuLayout::GetMountedExplorers() {
        return this->mounted_explorers;
    }

}