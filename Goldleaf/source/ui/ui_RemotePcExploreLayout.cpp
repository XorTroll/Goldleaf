
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

#include <ui/ui_RemotePcExploreLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <usb/cf/cf_CommandFramework.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void RemotePcExploreLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    RemotePcExploreLayout::RemotePcExploreLayout() : pu::ui::Layout() {
        this->paths_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->paths_menu);
        this->Add(this->paths_menu);

        this->SetOnInput(std::bind(&RemotePcExploreLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    Result RemotePcExploreLayout::Reload() {
        g_MainApplication->LoadMenuData(true, cfg::Strings.GetString(299), GetCommonIcon(CommonIconKind::Pc), cfg::Strings.GetString(152));
        ScopeGuard cancel_guard([this]() {
            g_MainApplication->PopMenuData();
        });

        SleepWhileRender(100'000'000);

        this->names.clear();
        this->paths.clear();
        this->paths_menu->ClearItems();

        u32 drive_count;
        auto rc = usb::cf::GetDriveCount(drive_count);
        if(R_SUCCEEDED(rc)) {
            for(u32 i = 0; i < drive_count; i++) {
                std::string label;
                std::string path;
                size_t tmp_total_size;
                size_t tmp_free_size;
                // TODO (low priority): make use of drive sizes?
                rc = usb::cf::GetDriveInfo(i, label, path, tmp_total_size, tmp_free_size);
                if(R_SUCCEEDED(rc)) {
                    this->names.push_back(label);
                    this->paths.push_back(path);
                }
                else {
                    HandleResult(rc, cfg::Strings.GetString(256));
                    return rc;
                }
            }
        }
        else {
            HandleResult(rc, cfg::Strings.GetString(256));
            return rc;
        }

        u32 path_count;
        rc = usb::cf::GetSpecialPathCount(path_count);
        if(R_SUCCEEDED(rc)) {
            for(u32 i = 0; i < path_count; i++) {
                std::string name;
                std::string path;
                rc = usb::cf::GetSpecialPath(i, name, path);
                if(R_SUCCEEDED(rc)) {
                    this->names.push_back(name);
                    this->paths.push_back(path);    
                }
                else {
                    HandleResult(rc, cfg::Strings.GetString(256));
                    return rc;
                }
            }
        }
        else {
            HandleResult(rc, cfg::Strings.GetString(256));
            return rc;
        }

        auto file_select_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(407));
        file_select_item->SetColor(g_Settings.GetColorScheme().text);
        file_select_item->SetIcon(GetCommonIcon(CommonIconKind::BinaryFile));
        file_select_item->AddOnKey(std::bind(&RemotePcExploreLayout::OnSelectFileSelected, this));
        this->paths_menu->AddItem(file_select_item);

        for(u32 i = 0; i < this->names.size(); i++) {
            auto item = pu::ui::elm::MenuItem::New(this->names[i]);
            item->SetColor(g_Settings.GetColorScheme().text);
            if(i < drive_count) {
                item->SetIcon(GetCommonIcon(CommonIconKind::Drive));
            }
            else {
                item->SetIcon(GetCommonIcon(CommonIconKind::Directory));
            }
            item->AddOnKey(std::bind(&RemotePcExploreLayout::OnPathSelected, this));
            this->paths_menu->AddItem(item);
        }

        this->paths_menu->SetSelectedIndex(0);
        cancel_guard.Cancel();
        g_MainApplication->LoadMenuHead(cfg::Strings.GetString(516));
        return rc;
    }

    void RemotePcExploreLayout::OnPathSelected() {
        const auto idx = this->paths_menu->GetSelectedIndex();
        g_MainApplication->LoadMenuData(true, cfg::Strings.GetString(299), GetCommonIcon(CommonIconKind::Drive), cfg::Strings.GetString(401));
        g_MainApplication->GetBrowserLayout()->ChangePartitionRemotePcDrive(this->paths[idx]);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void RemotePcExploreLayout::OnSelectFileSelected() {
        std::string selected_file;
        if(R_SUCCEEDED(usb::cf::SelectFile(selected_file))) {
            g_MainApplication->GetBrowserLayout()->HandleFileDirectly(selected_file);
        }
    }

}
