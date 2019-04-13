
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Goldleaf>
#include <pu/Plutonium>

namespace gleaf::ui
{
    class PartitionBrowserLayout : public pu::Layout
    {
        public:
            PartitionBrowserLayout();
            ~PartitionBrowserLayout();
            void ChangePartitionSdCard(bool Update = true);
            void ChangePartitionNAND(fs::Partition Partition, bool Update = true);
            void ChangePartitionPCDrive(std::string Mount, bool Update = true);
            void UpdateElements();
            bool GoBack();
            bool WarnNANDWriteAccess();
            void fsItems_Click();
            void fsItems_Click_Y();
            fs::Explorer *GetExplorer();
        private:
            fs::Explorer *gexp;
            std::vector<std::string> elems;
            pu::element::Menu *browseMenu;
            pu::element::TextBlock *dirEmptyText;
    };
}