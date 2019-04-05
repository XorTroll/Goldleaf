
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
    class SystemInfoLayout : public pu::Layout
    {
        public:
            SystemInfoLayout();
            void UpdateElements();
        private:
            pu::element::TextBlock *fwText;
            pu::element::TextBlock *sdText;
            pu::element::ProgressBar *sdBar;
            pu::element::TextBlock *sdFreeText;
            pu::element::TextBlock *nandText;
            pu::element::ProgressBar *nandBar;
            pu::element::TextBlock *nandFreeText;
            pu::element::TextBlock *safeText;
            pu::element::ProgressBar *safeBar;
            pu::element::TextBlock *systemText;
            pu::element::ProgressBar *systemBar;
            pu::element::TextBlock *userText;
            pu::element::ProgressBar *userBar;
    };
}