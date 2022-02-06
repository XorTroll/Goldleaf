
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

    PCExploreLayout::PCExploreLayout() : pu::ui::Layout() {
        this->paths_menu = pu::ui::elm::Menu::New(0, 160, pu::ui::render::ScreenWidth, g_Settings.custom_scheme.base, g_Settings.custom_scheme.base_focus, g_Settings.menu_item_size, ComputeDefaultMenuItemCount(g_Settings.menu_item_size));
        g_Settings.ApplyScrollBarColor(this->paths_menu);
        this->Add(this->paths_menu);
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
                    this->names.push_back(label + " (" + path + ":\\)");
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
            item->SetColor(g_Settings.custom_scheme.text);
            if(i < drive_count) {
                item->SetIcon(g_Settings.PathForResource("/Common/Drive.png"));
            }
            else {
                item->SetIcon(g_Settings.PathForResource("/FileSystem/Directory.png"));
            }
            item->AddOnKey(std::bind(&PCExploreLayout::path_DefaultKey, this));
            this->paths_menu->AddItem(item);
        }

        auto file_select_item = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(407));
        file_select_item->SetColor(g_Settings.custom_scheme.text);
        file_select_item->SetIcon(g_Settings.PathForResource("/FileSystem/File.png"));
        file_select_item->AddOnKey(std::bind(&PCExploreLayout::fileSelect_DefaultKey, this));
        this->paths_menu->AddItem(file_select_item);
        this->paths_menu->SetSelectedIndex(0);
    }

    void PCExploreLayout::path_DefaultKey() {
        const auto idx = this->paths_menu->GetSelectedIndex();
        g_MainApplication->GetBrowserLayout()->ChangePartitionPCDrive(this->paths[idx]);
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void PCExploreLayout::fileSelect_DefaultKey() {
        std::string selected_file;
        if(R_SUCCEEDED(usb::cf::SelectFile(selected_file))) {
            g_MainApplication->GetBrowserLayout()->HandleFileDirectly(selected_file);
        }
    }

}