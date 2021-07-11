
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

            void ChangePartitionExplorer(fs::Explorer *exp, bool update_contents = true);
            void ChangePartitionSdCard(bool update_contents = true);
            void ChangePartitionNAND(fs::Partition partition, bool update_contents = true);
            void ChangePartitionPCDrive(String mount_name, bool update_contents = true);
            void ChangePartitionDrive(UsbHsFsDevice &drv, bool update_contents = true);
            void UpdateElements(int idx = 0);
            void HandleFileDirectly(String path);
            bool GoBack();
            bool WarnWriteAccess();
            void fsItems_Click(String item);
            void fsItems_Click_Y(String item);
            
            inline fs::Explorer *GetExplorer() {
                return this->cur_exp;
            }
    };

}