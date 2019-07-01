
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <ui/ui_Includes.hpp>
#include <pu/Plutonium>

namespace ui
{
    class ExploreMenuLayout : public pu::ui::Layout
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
            pu::ui::elm::Menu *mountsMenu;
            pu::ui::elm::MenuItem *sdCardMenuItem;
            pu::ui::elm::MenuItem *pcDriveMenuItem;
            pu::ui::elm::MenuItem *usbDriveMenuItem;
            pu::ui::elm::MenuItem *nandProfInfoFMenuItem;
            pu::ui::elm::MenuItem *nandSafeMenuItem;
            pu::ui::elm::MenuItem *nandUserMenuItem;
            pu::ui::elm::MenuItem *nandSystemMenuItem;
            std::vector<pu::ui::elm::MenuItem*> mounts;
            std::vector<fs::Explorer*> expls;
    };
}