
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
    class CopyLayout : public pu::ui::Layout
    {
        public:
            CopyLayout();
            ~CopyLayout();
            void StartCopy(std::string Path, std::string NewPath, bool Directory, fs::Explorer *Exp, pu::ui::Layout *Prev);
        private:
            fs::Explorer *gexp;
            pu::ui::elm::TextBlock *infoText;
            pu::ui::elm::ProgressBar *copyBar;
    };
}