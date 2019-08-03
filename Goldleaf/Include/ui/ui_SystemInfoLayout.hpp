
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
    class SystemInfoLayout : public pu::ui::Layout
    {
        public:
            SystemInfoLayout();
            ~SystemInfoLayout();
            void UpdateElements();
        private:
            pu::ui::elm::TextBlock *fwText;
            pu::ui::elm::TextBlock *sdText;
            pu::ui::elm::ProgressBar *sdBar;
            pu::ui::elm::TextBlock *sdFreeText;
            pu::ui::elm::TextBlock *nandText;
            pu::ui::elm::ProgressBar *nandBar;
            pu::ui::elm::TextBlock *nandFreeText;
            pu::ui::elm::TextBlock *safeText;
            pu::ui::elm::ProgressBar *safeBar;
            pu::ui::elm::TextBlock *systemText;
            pu::ui::elm::ProgressBar *systemBar;
            pu::ui::elm::TextBlock *userText;
            pu::ui::elm::ProgressBar *userBar;
    };
}