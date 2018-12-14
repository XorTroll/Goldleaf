
#pragma once
#include <pn/fw/Element.hpp>

namespace pn::fw
{
    class ProgressBar : public Element
    {
        public:
            ProgressBar(u32 X, u32 Y, u32 Width, u32 Height);
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
            u8 GetProgress();
            void SetProgress(u8 Percentage);
            void IncrementProgress(u8 Percentage);
            void DecrementProgress(u8 Percentage);
            void FillProgress();
            void ClearProgress();
            bool IsCompleted();
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Input);
        private:
            u32 x;
            u32 y;
            u32 w;
            u32 h;
            u8 perc;
            draw::Color clr;
            draw::Color oclr;
    };
}