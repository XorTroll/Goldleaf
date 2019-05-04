
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
    class USBDrivesLayout : public pu::Layout
    {
        public:
            USBDrivesLayout();
            ~USBDrivesLayout();
            void Start();
            void UpdateDrives();
            void drive_Click();
            std::vector<drive::Drive*> GetDrives();
        private:
            pu::element::Menu *drivesMenu;
            pu::element::TextBlock *noDrivesText;
            std::vector<drive::Drive*> drvs;
    };
}