
/*

    Plutonium library

    @file Rectangle.hpp
    @brief A Rectangle is an Element which simply draws a filled rectangle.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element/Element.hpp>

namespace pu::element
{
    class Rectangle : public Element
    {
        public:
            Rectangle(u32 X, u32 Y, u32 Width, u32 Height, draw::Color RecColor, u32 BorderRadius = 0);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            void SetWidth(u32 Width);
            u32 GetHeight();
            void SetHeight(u32 Height);
            u32 GetBorderRadius();
            void SetBorderRadius(u32 Radius);
            draw::Color GetColor();
            void SetColor(draw::Color RecColor);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch, bool Focus);
        private:
            u32 borderr;
            u32 x;
            u32 y;
            u32 w;
            u32 h;
            draw::Color clr;
    };
}