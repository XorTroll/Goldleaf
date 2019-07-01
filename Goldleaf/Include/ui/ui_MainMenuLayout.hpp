
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
    class MainMenuLayout : public pu::ui::Layout
    {
        public:
            MainMenuLayout();
            ~MainMenuLayout();
            void exploreMenuItem_Click();
            void titleMenuItem_Click();
            void webMenuItem_Click();
            void accountMenuItem_Click();
            void amiiboMenuItem_Click();
            void sysinfoMenuItem_Click();
            void updateMenuItem_Click();
            void aboutMenuItem_Click();
        private:
            pu::ui::elm::Menu *optionMenu;
            pu::ui::elm::MenuItem *exploreMenuItem;
            pu::ui::elm::MenuItem *titleMenuItem;
            pu::ui::elm::MenuItem *webMenuItem;
            pu::ui::elm::MenuItem *accountMenuItem;
            pu::ui::elm::MenuItem *amiiboMenuItem;
            pu::ui::elm::MenuItem *sysinfoMenuItem;
            pu::ui::elm::MenuItem *updateMenuItem;
            pu::ui::elm::MenuItem *aboutMenuItem;
    };
}