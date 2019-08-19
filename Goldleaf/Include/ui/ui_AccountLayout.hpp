
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
    class AccountLayout : public pu::ui::Layout
    {
        public:
            AccountLayout();
            ~AccountLayout();
            void ReloadItems();
            void Load();
            void optsRename_Click();
            void optsIcon_Click();
            void optsDelete_Click();
            void optsServicesInfo_Click();
        private:
            pu::ui::elm::Menu *optsMenu;
            u128 uid;
    };
}