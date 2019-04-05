
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
    class FileContentLayout : public pu::Layout
    {
        public:
            FileContentLayout();
            void LoadFile(std::string Path, pu::Layout *Prev, fs::Explorer *Exp, bool Hex);
            void Update();
            void ScrollUp();
            void ScrollDown();
            pu::Layout *GetPreviousLayout();
        private:
            u32 loffset;
            u32 rlines;
            bool mode;
            std::string pth;
            pu::element::TextBlock *cntText;
            fs::Explorer *gexp;
            pu::Layout *prev;
    };
}