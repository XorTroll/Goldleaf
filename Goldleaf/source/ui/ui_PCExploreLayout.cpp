
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

#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_MainApplication.hpp>

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
        u32 drive_count = 0;
        auto rc = usb::ProcessCommand<usb::CommandId::GetDriveCount>(usb::OutValue(drive_count));
        if(R_SUCCEEDED(rc)) {
            for(u32 i = 0; i < drive_count; i++) {
                std::string label;
                std::string path;
                u32 tmp_total_size = 0;
                u32 tmp_free_size = 0;
                // TODO (low priority): make use of drive sizes?
                rc = usb::ProcessCommand<usb::CommandId::GetDriveInfo>(usb::InValue(i), usb::OutString(label), usb::OutString(path), usb::OutValue(tmp_total_size), usb::OutValue(tmp_free_size));
                if(R_SUCCEEDED(rc)) {
                    this->names.push_back(label + " (" + path + ":\\)");
                    this->paths.push_back(path);
                }
            }
        }
        u32 path_count = 0;
        rc = usb::ProcessCommand<usb::CommandId::GetSpecialPathCount>(usb::OutValue(path_count));
        if(R_SUCCEEDED(rc)) {
            for(u32 i = 0; i < path_count; i++) {
                std::string name;
                std::string path;
                rc = usb::ProcessCommand<usb::CommandId::GetSpecialPath>(usb::InValue(i), usb::OutString(name), usb::OutString(path));
                if(R_SUCCEEDED(rc)) {
                    this->names.push_back(name);
                    this->paths.push_back(path);    
                }
            }
        }
        for(u32 i = 0; i < this->names.size(); i++) {
            auto itm = pu::ui::elm::MenuItem::New(this->names[i]);
            itm->SetColor(g_Settings.custom_scheme.text);
            if(i < drive_count) {
                itm->SetIcon(g_Settings.PathForResource("/Common/Drive.png"));
            }
            else {
                itm->SetIcon(g_Settings.PathForResource("/FileSystem/Directory.png"));
            }
            itm->AddOnKey(std::bind(&PCExploreLayout::path_DefaultKey, this));
            this->paths_menu->AddItem(itm);
        }
        auto file_select_itm = pu::ui::elm::MenuItem::New(cfg::strings::Main.GetString(407));
        file_select_itm->SetColor(g_Settings.custom_scheme.text);
        file_select_itm->SetIcon(g_Settings.PathForResource("/FileSystem/File.png"));
        file_select_itm->AddOnKey(std::bind(&PCExploreLayout::fileSelect_DefaultKey, this));
        this->paths_menu->AddItem(file_select_itm);
        this->paths_menu->SetSelectedIndex(0);
    }

    void PCExploreLayout::path_DefaultKey() {
        const auto idx = this->paths_menu->GetSelectedIndex();
        g_MainApplication->GetBrowserLayout()->ChangePartitionPCDrive(this->paths[idx]);
        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
    }

    void PCExploreLayout::fileSelect_DefaultKey() {
        std::string selected_file;
        const auto rc = usb::ProcessCommand<usb::CommandId::SelectFile>(usb::OutString(selected_file));
        if(R_SUCCEEDED(rc)) {
            g_MainApplication->GetBrowserLayout()->HandleFileDirectly(selected_file);
        }
    }

}