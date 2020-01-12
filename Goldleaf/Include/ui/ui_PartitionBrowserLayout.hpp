
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

#pragma once
#include <ui/ui_Includes.hpp>
#include <pu/Plutonium>

namespace ui
{
    class PartitionBrowserLayout : public pu::ui::Layout
    {
        public:
            PartitionBrowserLayout();
            PU_SMART_CTOR(PartitionBrowserLayout)

            void ChangePartitionExplorer(fs::Explorer *exp, bool Update = true);
            void ChangePartitionSdCard(bool Update = true);
            void ChangePartitionNAND(fs::Partition Partition, bool Update = true);
            void ChangePartitionPCDrive(String Mount, bool Update = true);
            void ChangePartitionUSBDrive(drive::Drive drv, bool Update = true);
            void UpdateElements(int Idx = 0);
            void HandleFileDirectly(String Path);
            bool GoBack();
            bool WarnNANDWriteAccess();
            void fsItems_Click(String item);
            void fsItems_Click_Y(String item);
            fs::Explorer *GetExplorer();
        private:
            fs::Explorer *gexp;
            std::vector<String> elems;
            pu::ui::elm::Menu::Ref browseMenu;
            pu::ui::elm::TextBlock::Ref dirEmptyText;
    };
}