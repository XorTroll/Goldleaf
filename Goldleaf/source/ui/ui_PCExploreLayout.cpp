
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

#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <usb/cf/cf_CommandFramework.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    void PCExploreLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            g_MainApplication->ReturnToParentLayout();
        }
    }

    PCExploreLayout::PCExploreLayout() : pu::ui::Layout() {
        this->paths_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        this->paths_menu->SetScrollbarColor(g_Settings.GetColorScheme().scroll_bar);
        this->Add(this->paths_menu);

        this->SetOnInput(std::bind(&PCExploreLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void PCExploreLayout::UpdatePaths() {
        this->names.clear();
        this->paths.clear();
        this->paths_menu->ClearItems();

        u32 drive_count;
        if(R_SUCCEEDED(usb::cf::GetDriveCount(drive_count))) {
            for(u32 i = 0; i < drive_count; i++) {
                std::string label;
                std::string path;
                size_t tmp_total_size;
                size_t tmp_free_size;
                // TODO (low priority): make use of drive sizes?
                if(R_SUCCEEDED(usb::cf::GetDriveInfo(i, label, path, tmp_total_size, tmp_free_size))) {
                    this->names.push_back(label);
                    this->paths.push_back(path);
                }
            }
        }

        u32 path_count;
        if(R_SUCCEEDED(usb::cf::GetSpecialPathCount(path_count))) {
            for(u32 i = 0; i < path_count; i++) {
                std::string name;
                std::string path;
                if(R_SUCCEEDED(usb::cf::GetSpecialPath(i, name, path))) {
                    this->names.push_back(name);
                    this->paths.push_back(path);    
                }
            }
        }

        for(u32 i = 0; i < this->names.size(); i++) {
            auto item = pu::ui::elm::MenuItem::New(this->names[i]);
            item->SetColor(g_Settings.GetColorScheme().text);
            if(i < drive_count) {
                item->SetIcon(GetCommonIcon(CommonIconKind::Drive));
            }
            else {
                item->SetIcon(GetCommonIcon(CommonIconKind::Directory));
            }
            item->AddOnKey(std::bind(&PCExploreLayout::path_DefaultKey, this));
            this->paths_menu->AddItem(item);
        }

        auto file_select_item = pu::ui::elm::MenuItem::New(cfg::Strings.GetString(407));
        file_select_item->SetColor(g_Settings.GetColorScheme().text);
        file_select_item->SetIcon(GetCommonIcon(CommonIconKind::BinaryFile));
        file_select_item->AddOnKey(std::bind(&PCExploreLayout::fileSelect_DefaultKey, this));
        this->paths_menu->AddItem(file_select_item);
        this->paths_menu->SetSelectedIndex(0);
    }

    void PCExploreLayout::path_DefaultKey() {
        const auto idx = this->paths_menu->GetSelectedIndex();
        g_MainApplication->GetBrowserLayout()->ChangePartitionPCDrive(this->paths[idx]);
        g_MainApplication->ShowLayout(g_MainApplication->GetBrowserLayout());
    }

    void PCExploreLayout::fileSelect_DefaultKey() {
        std::string selected_file;
        if(R_SUCCEEDED(usb::cf::SelectFile(selected_file))) {
            g_MainApplication->GetBrowserLayout()->HandleFileDirectly(selected_file);
        }
    }

}
