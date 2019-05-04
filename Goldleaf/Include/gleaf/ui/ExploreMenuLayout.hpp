
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
    class ExploreMenuLayout : public pu::Layout
    {
        public:
            ExploreMenuLayout();
            ~ExploreMenuLayout();
            void mountsMenu_SelectionChanged();
            void sdCard_Click();
            void pcDrive_Click();
            void usbDrive_Click();
            void nandProdInfoF_Click();
            void nandSafe_Click();
            void nandUser_Click();
            void nandSystem_Click();
            void otherMount_Click();
            void specialMount_Click_X();
            void otherMount_Click_X();
        private:
            pu::element::Menu *mountsMenu;
            pu::element::MenuItem *sdCardMenuItem;
            pu::element::MenuItem *pcDriveMenuItem;
            pu::element::MenuItem *usbDriveMenuItem;
            pu::element::MenuItem *nandProfInfoFMenuItem;
            pu::element::MenuItem *nandSafeMenuItem;
            pu::element::MenuItem *nandUserMenuItem;
            pu::element::MenuItem *nandSystemMenuItem;
            std::vector<pu::element::MenuItem*> mounts;
            std::vector<fs::Explorer*> expls;
            std::vector<drive::Drive*> drvs;
    };
}