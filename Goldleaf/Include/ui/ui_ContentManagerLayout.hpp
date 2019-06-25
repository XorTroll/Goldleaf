
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
    class ContentManagerLayout : public pu::Layout
    {
        public:
            ContentManagerLayout();
            ~ContentManagerLayout();
            void sdCardMenuItem_Click();
            void nandUserMenuItem_Click();
            void nandSystemMenuItem_Click();
            void gameCartMenuItem_Click();
            void unusedTicketsMenuItem_Click();
        private:
            pu::element::MenuItem *sdCardMenuItem;
            pu::element::MenuItem *nandUserMenuItem;
            pu::element::MenuItem *nandSystemMenuItem;
            pu::element::MenuItem *gameCartMenuItem;
            pu::element::MenuItem *unusedTicketsMenuItem;
            pu::element::Menu *typesMenu;
    };
}