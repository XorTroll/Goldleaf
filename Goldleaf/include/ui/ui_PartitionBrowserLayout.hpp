
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

#pragma once
#include <ui/ui_Includes.hpp>

namespace ui {

    class PartitionBrowserLayout : public pu::ui::Layout {
        private:
            fs::Explorer *cur_exp;
            pu::ui::elm::Menu::Ref browse_menu;
            pu::ui::elm::TextBlock::Ref empty_dir_text;

        public:
            PartitionBrowserLayout();
            PU_SMART_CTOR(PartitionBrowserLayout)

            void ChangePartitionExplorer(fs::Explorer *exp, const bool update_contents = true);
            void ChangePartitionSdCard(const bool update_contents = true);
            void ChangePartitionNAND(const fs::Partition partition, const bool update_contents = true);
            void ChangePartitionPCDrive(const std::string &mount_name, const bool update_contents = true);
            void ChangePartitionDrive(UsbHsFsDevice &drv, const bool update_contents = true);
            void UpdateElements(const int idx = 0);
            void HandleFileDirectly(const std::string &path);
            bool GoBack();
            bool WarnWriteAccess();
            void fsItems_DefaultKey(const std::string &item);
            void fsItems_Y(const std::string &item);
            
            inline fs::Explorer *GetExplorer() {
                return this->cur_exp;
            }
    };

}