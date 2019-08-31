
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
            PU_SMART_CTOR(FileContentLayout)

            void LoadFile(pu::String PPath, pu::String Path, fs::Explorer *Exp, bool Hex);
            void Update();
            void ScrollUp();
            void ScrollDown();
        private:
            u32 loffset;
            u32 rlines;
            bool mode;
            pu::String pth;
            pu::ui::elm::TextBlock::Ref cntText;
            fs::Explorer *gexp;
    };
}