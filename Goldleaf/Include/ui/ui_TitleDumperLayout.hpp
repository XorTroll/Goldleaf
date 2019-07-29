
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
    class TitleDumperLayout : public pu::ui::Layout
    {
        public:
            TitleDumperLayout();
            ~TitleDumperLayout();
            void StartDump(hos::Title &Target);
        private:
            pu::ui::elm::TextBlock *dumpText;
            pu::ui::elm::ProgressBar *ncaBar;
    };
}