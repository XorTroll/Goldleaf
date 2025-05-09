
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <usb/usb_Base.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void ExploreMenuLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout() {
        this->mounts_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->mounts_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        this->Add(this->mounts_menu);

        this->SetOnInput(std::bind(&ExploreMenuLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void ExploreMenuLayout::UpdateMenu() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(277), CommonIconKind::Storage, cfg::Strings.GetString(278));

        this->mounts_menu->ClearItems();
        this->sd_card_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(19));
        this->sd_card_menu_item->SetIcon(GetCommonIcon(CommonIconKind::SdCard));
        this->sd_card_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->sd_card_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::sdCard_DefaultKey, this));
        this->remote_pc_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(279));
        this->remote_pc_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Drive));
        this->remote_pc_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->remote_pc_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::pcDrive_DefaultKey, this));
        this->usb_drive_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(398));
        this->usb_drive_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Drive));
        this->usb_drive_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->usb_drive_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::usbDrive_DefaultKey, this));
        this->nand_prodinfof_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (PRODINFOF)");
        this->nand_prodinfof_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_prodinfof_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_prodinfof_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandProdInfoF_DefaultKey, this));
        this->nand_safe_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (SAFE)");
        this->nand_safe_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_safe_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_safe_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandSafe_DefaultKey, this));
        this->nand_user_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (USER)");
        this->nand_user_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_user_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_user_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::nandUser_DefaultKey, this));
        this->nand_system_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (SYSTEM)");
        this->nand_system_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_system_menu_item->SetColor(g_Settings.GetColorScheme().text);
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
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(19), CommonIconKind::SdCard, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionSdCard();
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::pcDrive_DefaultKey() {
        if(usb::IsStateOk()) {
            g_MainApplication->GetPCExploreLayout()->UpdatePaths();
            g_MainApplication->ShowLayout(g_MainApplication->GetPCExploreLayout());
        }
        else {
            g_MainApplication->DisplayDialog(cfg::Strings.GetString(299), cfg::Strings.GetString(300), { cfg::Strings.GetString(234) }, true);
        }
    }

    void ExploreMenuLayout::usbDrive_DefaultKey() {
        auto drives = drive::ListDrives();
        std::vector<std::string> options;
        for(const auto &drive: drives) {
            options.push_back(drive.name);
        }
        options.push_back(cfg::Strings.GetString(18));

        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(401), cfg::Strings.GetString(402) + " " + std::to_string(drives.size()), options, true);
        if(!drives.empty()) {
            if(option_1 < static_cast<s64>(drives.size())) {
                auto &drive = drives.at(option_1);
                const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(401), cfg::Strings.GetString(434), { cfg::Strings.GetString(136), cfg::Strings.GetString(435), cfg::Strings.GetString(18) }, true);
                if(option_2 == 0) {
                    g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(403), CommonIconKind::Drive, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
                    g_MainApplication->GetBrowserLayout()->ChangePartitionDrive(drive);
                    g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
                }
                else if(option_2 == 1) {
                    if(drive::UnmountDrive(drive)) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(436));
                    }
                    else {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(437));
                    }
                }
            }
        }
    }

    void ExploreMenuLayout::nandProdInfoF_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(1), CommonIconKind::NAND, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::PRODINFOF);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSafe_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(1), CommonIconKind::NAND, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSafe);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandUser_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(1), CommonIconKind::NAND, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDUser);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::nandSystem_DefaultKey() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(1), CommonIconKind::NAND, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(fs::Partition::NANDSystem);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_DefaultKey(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon) {
        g_MainApplication->LoadMenuData(true, name, icon, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionExplorer(exp);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::explorer_X(fs::Explorer *exp) {
        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(404), cfg::Strings.GetString(405), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
        if(option == 0) {
            auto mounted_exp = std::find(this->mounted_explorers.begin(), this->mounted_explorers.end(), exp);
            if(mounted_exp != this->mounted_explorers.end()) {
                delete exp;
                this->mounted_explorers.erase(mounted_exp);
                const auto idx = std::distance(this->mounted_explorers.begin(), mounted_exp);
                this->mounted_explorer_items.erase(this->mounted_explorer_items.begin() + idx);
                this->UpdateMenu();
                g_MainApplication->ShowNotification(cfg::Strings.GetString(406));
            }
            else {
                // TODO (low priority): show anything if not found? (should never happen...)
            }
        }
    }

    void ExploreMenuLayout::AddMountedExplorer(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon) {
        auto mounted_exp_item = pu::ui::elm::MenuItem::New(name);
        mounted_exp_item->SetIcon(icon);
        mounted_exp_item->SetColor(g_Settings.GetColorScheme().text);
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::explorer_DefaultKey, this, exp, name, icon));
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::explorer_X, this, exp), HidNpadButton_X);
        this->mounted_explorer_items.push_back(mounted_exp_item);
        this->mounted_explorers.push_back(exp);
        fs::RegisterMountedExplorer(exp);
        this->UpdateMenu();
    }

}
