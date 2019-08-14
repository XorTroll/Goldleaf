
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
    class InstallLayout : public pu::ui::Layout
    {
        public:
            InstallLayout();
            ~InstallLayout();
            void StartInstall(std::string Path, fs::Explorer *Exp, Storage Location, pu::ui::Layout *Prev, bool OmitConfirmation = false);
        private:
            pu::ui::elm::TextBlock *installText;
            pu::ui::elm::ProgressBar *installBar;
    };
}