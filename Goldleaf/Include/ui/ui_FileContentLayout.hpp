
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
    class FileContentLayout : public pu::ui::Layout
    {
        public:
            FileContentLayout();
            ~FileContentLayout();
            void LoadFile(pu::String Path, pu::ui::Layout *Prev, fs::Explorer *Exp, bool Hex);
            void Update();
            void ScrollUp();
            void ScrollDown();
            pu::ui::Layout *GetPreviousLayout();
        private:
            u32 loffset;
            u32 rlines;
            bool mode;
            pu::String pth;
            pu::ui::elm::TextBlock *cntText;
            fs::Explorer *gexp;
            pu::ui::Layout *prev;
    };
}