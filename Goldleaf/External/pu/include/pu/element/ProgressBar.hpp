
/*

    Plutonium library

    @file ProgressBar.hpp
    @brief A ProgressBar is an Element which represents a progress (a percentage) by filling a bar.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element/Element.hpp>

namespace pu::element
{
    class ProgressBar : public Element
    {
        public:
            ProgressBar(u32 X, u32 Y, u32 Width, u32 Height, double MaxValue);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            void SetWidth(u32 Width);
            u32 GetHeight();
            void SetHeight(u32 Height);
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            draw::Color GetProgressColor();
            void SetProgressColor(draw::Color Color);
            double GetProgress();
            void SetProgress(double Progress);
            void IncrementProgress(double Progress);
            void DecrementProgress(double Progress);
            void SetMaxValue(double Max);
            double GetMaxValue();
            void FillProgress();
            void ClearProgress();
            bool IsCompleted();
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch, bool Focus);
        private:
            u32 x;
            u32 y;
            u32 w;
            u32 h;
            double val;
            double maxval;
            draw::Color clr;
            draw::Color oclr;
    };
}