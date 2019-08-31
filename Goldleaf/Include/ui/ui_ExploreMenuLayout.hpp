
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
            PU_SMART_CTOR(ExploreMenuLayout)

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
            pu::ui::elm::Menu::Ref mountsMenu;
            pu::ui::elm::MenuItem::Ref sdCardMenuItem;
            pu::ui::elm::MenuItem::Ref pcDriveMenuItem;
            pu::ui::elm::MenuItem::Ref usbDriveMenuItem;
            pu::ui::elm::MenuItem::Ref nandProfInfoFMenuItem;
            pu::ui::elm::MenuItem::Ref nandSafeMenuItem;
            pu::ui::elm::MenuItem::Ref nandUserMenuItem;
            pu::ui::elm::MenuItem::Ref nandSystemMenuItem;
            std::vector<pu::ui::elm::MenuItem::Ref> mounts;
            std::vector<fs::Explorer*> expls;
    };
}