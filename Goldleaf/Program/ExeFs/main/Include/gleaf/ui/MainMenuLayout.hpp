
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
    class MainMenuLayout : public pu::Layout
    {
        public:
            MainMenuLayout();
            void optionMenu_SelectionChanged();
            void sdcardMenuItem_Click();
            void nandMenuItem_Click();
            void pcdriveMenuItem_Click();
            void usbMenuItem_Click();
            void titleMenuItem_Click();
            void ticketMenuItem_Click();
            void webMenuItem_Click();
            void accountMenuItem_Click();
            void sysinfoMenuItem_Click();
            void updateMenuItem_Click();
            void aboutMenuItem_Click();
        private:
            pu::element::Menu *optionMenu;
            pu::element::MenuItem *sdcardMenuItem;
            pu::element::MenuItem *nandMenuItem;
            pu::element::MenuItem *pcdriveMenuItem;
            pu::element::MenuItem *usbMenuItem;
            pu::element::MenuItem *titleMenuItem;
            pu::element::MenuItem *ticketMenuItem;
            pu::element::MenuItem *webMenuItem;
            pu::element::MenuItem *accountMenuItem;
            pu::element::MenuItem *sysinfoMenuItem;
            pu::element::MenuItem *updateMenuItem;
            pu::element::MenuItem *aboutMenuItem;
    };
}