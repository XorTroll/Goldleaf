
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
#include <usb/cf/cf_CommandFramework.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void ExploreMenuLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(drive::GetConsumeDrivesChanged() || (keys_down & HidNpadButton_Y)) {
            this->ReloadMenu();
        }

        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    ExploreMenuLayout::ExploreMenuLayout() : pu::ui::Layout() {
        this->mounts_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->mounts_menu);
        this->Add(this->mounts_menu);

        this->SetOnInput(std::bind(&ExploreMenuLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void ExploreMenuLayout::Reload() {
        this->ReloadMenu();
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(277), CommonIconKind::Storage, cfg::Strings.GetString(278));
    }

    void ExploreMenuLayout::ReloadMenu() {
        this->mounts_menu->ClearItems();

        this->sd_card_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(19));
        this->sd_card_menu_item->SetIcon(GetCommonIcon(CommonIconKind::SdCard));
        this->sd_card_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->sd_card_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnSdCardSelected, this));

        this->usb_drive_menu_items.clear();
        drive::DoWithDrives([this](const std::vector<UsbHsFsDevice> &drives) {
            for(const auto &drive: drives) {
                auto drive_item = pu::ui::elm::MenuItem::New("USB drive: " + drive::FormatDriveName(drive));
                drive_item->SetIcon(GetCommonIcon(CommonIconKind::USB));
                drive_item->SetColor(g_Settings.GetColorScheme().text);
                auto drive_copy = drive;
                drive_item->AddOnKey(std::bind(&ExploreMenuLayout::OnUsbDriveSelected, this, drive_copy));
                drive_item->AddOnKey(std::bind(&ExploreMenuLayout::OnUsbDriveSelectedX, this, drive_copy), HidNpadButton_X);
                this->usb_drive_menu_items.push_back(drive_item);
            }
        });

        this->remote_pc_special_menu_items.clear();
        this->remote_pc_special_paths.clear();
        auto usb_ok = usb::IsStateOk();

        u32 drive_count;
        u32 path_count;
        if(usb_ok) {
            const auto rc = usb::cf::GetDriveCount(drive_count);
            if(R_SUCCEEDED(rc)) {
                for(u32 i = 0; i < drive_count; i++) {
                    std::string label;
                    std::string path;
                    size_t tmp_total_size;
                    size_t tmp_free_size;
                    // TODO (low priority): make use of drive sizes?
                    if(R_SUCCEEDED(usb::cf::GetDriveInfo(i, label, path, tmp_total_size, tmp_free_size))) {
                        this->remote_pc_special_paths.push_back({ label, path });
                    }
                }
            }
            else {
                usb_ok = false;
                this->remote_pc_special_menu_items.clear();
                this->remote_pc_special_paths.clear();
                HandleResult(rc, "USB error");
            }
        }

        if(usb_ok) {
            const auto rc = usb::cf::GetSpecialPathCount(path_count);
            if(R_SUCCEEDED(rc)) {
                for(u32 i = 0; i < path_count; i++) {
                    std::string name;
                    std::string path;
                    if(R_SUCCEEDED(usb::cf::GetSpecialPath(i, name, path))) {
                        this->remote_pc_special_paths.push_back({ name, path });   
                    }
                }
            }
            else {
                usb_ok = false;
                this->remote_pc_special_menu_items.clear();
                this->remote_pc_special_paths.clear();
                HandleResult(rc, "USB error");
            }
        }

        if(usb_ok) {
            u32 i = 0;
            for(const auto &path: this->remote_pc_special_paths) {
                pu::ui::elm::MenuItem::Ref item;
                if(i < drive_count) {
                    item = pu::ui::elm::MenuItem::New("PC drive: " + path.name);
                    item->SetIcon(GetCommonIcon(CommonIconKind::Drive));
                }
                else {
                    item = pu::ui::elm::MenuItem::New("PC path: " + path.name);
                    item->SetIcon(GetCommonIcon(CommonIconKind::Directory));
                }
                item->SetColor(g_Settings.GetColorScheme().text);
                item->AddOnKey(std::bind(&ExploreMenuLayout::OnRemotePcSpecialPathSelected, this, path));
                this->remote_pc_special_menu_items.push_back(item);
                i++;
            }

            this->remote_pc_select_file_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(407));
            this->remote_pc_select_file_menu_item->SetIcon(GetCommonIcon(CommonIconKind::Directory));
            this->remote_pc_select_file_menu_item->SetColor(g_Settings.GetColorScheme().text);
            this->remote_pc_select_file_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnRemotePcSelectFileSelected, this));
        }

        // Mounted explorer items are created elsewhere

        this->nand_user_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (USER)");
        this->nand_user_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_user_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_user_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnNandPartitionSelected, this, fs::Partition::NANDUser));

        this->nand_system_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (SYSTEM)");
        this->nand_system_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_system_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_system_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnNandPartitionSelected, this, fs::Partition::NANDSystem));

        this->nand_prodinfof_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (PRODINFOF)");
        this->nand_prodinfof_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_prodinfof_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_prodinfof_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnNandPartitionSelected, this, fs::Partition::PRODINFOF));

        this->nand_safe_menu_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(20) + " (SAFE)");
        this->nand_safe_menu_item->SetIcon(GetCommonIcon(CommonIconKind::NAND));
        this->nand_safe_menu_item->SetColor(g_Settings.GetColorScheme().text);
        this->nand_safe_menu_item->AddOnKey(std::bind(&ExploreMenuLayout::OnNandPartitionSelected, this, fs::Partition::NANDSafe));

        this->mounts_menu->AddItem(this->sd_card_menu_item);
        for(auto &drive_item: this->usb_drive_menu_items) {
            this->mounts_menu->AddItem(drive_item);
        }
        if(usb_ok) {
            for(auto &remote_pc_item: this->remote_pc_special_menu_items) {
                this->mounts_menu->AddItem(remote_pc_item);
            }
            this->mounts_menu->AddItem(this->remote_pc_select_file_menu_item);
        }
        for(auto &[exp, exp_item]: this->mounted_explorer_items) {
            this->mounts_menu->AddItem(exp_item);
        }
        this->mounts_menu->AddItem(this->nand_prodinfof_menu_item);
        this->mounts_menu->AddItem(this->nand_safe_menu_item);
        this->mounts_menu->AddItem(this->nand_user_menu_item);
        this->mounts_menu->AddItem(this->nand_system_menu_item);
        this->mounts_menu->SetSelectedIndex(0);
    }

    void ExploreMenuLayout::OnSdCardSelected() {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(19), CommonIconKind::SdCard, "");
        g_MainApplication->GetBrowserLayout()->ChangePartitionSdCard();
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::OnUsbDriveSelected(const UsbHsFsDevice drv) {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(403), CommonIconKind::Drive, "");
        g_MainApplication->GetBrowserLayout()->ChangePartitionDrive(drv);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::OnUsbDriveSelectedX(const UsbHsFsDevice drv) {
        std::string info;
        info += std::string("Filesystem type: ") + drive::FormatDriveFileSystemType(drv) + "\n";

        const auto option = g_MainApplication->DisplayDialog(drive::FormatDriveName(drv), info, { cfg::Strings.GetString(234), "Unmount" }, false);
        if(option == 1) {
            if(drive::UnmountDrive(drv)) {
                this->ReloadMenu();
                g_MainApplication->ShowNotification("Drive unmounted! You can unplug it safely.");
            }
            else {
                g_MainApplication->ShowNotification("Some error occurred while unmounting the drive...");
            }
        }
    }

    void ExploreMenuLayout::OnRemotePcSpecialPathSelected(const PcSpecialPath &path) {
        g_MainApplication->LoadCommonIconMenuData(true, "PC explorer", CommonIconKind::Directory, "");
        g_MainApplication->GetBrowserLayout()->ChangePartitionPCDrive(path.path);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::OnRemotePcSelectFileSelected() {
        std::string selected_file;
        if(R_SUCCEEDED(usb::cf::SelectFile(selected_file))) {
            g_MainApplication->GetBrowserLayout()->HandleFileDirectly(selected_file);
        }
        else {
            this->ReloadMenu();
        }
    }

    void ExploreMenuLayout::OnNandPartitionSelected(const fs::Partition partition) {
        g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(1), CommonIconKind::NAND, "");
        g_MainApplication->GetBrowserLayout()->ChangePartitionNAND(partition);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::OnMountedExplorerSelected(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon) {
        g_MainApplication->LoadMenuData(true, name, icon, g_MainApplication->GetBrowserLayout()->GetExplorer()->GetPresentableCwd());
        g_MainApplication->GetBrowserLayout()->ChangePartitionExplorer(exp);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void ExploreMenuLayout::OnMountedExplorerSelectedX(fs::Explorer *exp) {
        const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(404), cfg::Strings.GetString(405), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
        if(option == 0) {
            fs::UnregisterMountedExplorer(exp);
            this->mounted_explorer_items.erase(exp);
            this->ReloadMenu();
            g_MainApplication->ShowNotification(cfg::Strings.GetString(406));
        }
    }

    void ExploreMenuLayout::AddMountedExplorer(fs::Explorer *exp, const std::string &name, pu::sdl2::TextureHandle::Ref icon) {
        auto mounted_exp_item = pu::ui::elm::MenuItem::New(name);
        mounted_exp_item->SetIcon(icon);
        mounted_exp_item->SetColor(g_Settings.GetColorScheme().text);
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::OnMountedExplorerSelected, this, exp, name, icon));
        mounted_exp_item->AddOnKey(std::bind(&ExploreMenuLayout::OnMountedExplorerSelectedX, this, exp), HidNpadButton_X);
        this->mounted_explorer_items[exp] = mounted_exp_item;
        fs::RegisterMountedExplorer(exp);
        this->ReloadMenu();
    }

}
