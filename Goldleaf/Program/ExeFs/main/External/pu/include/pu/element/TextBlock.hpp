
/*

    Plutonium library

    @file TextBlock.hpp
    @brief A TextBlock is a very useful Element which is used to draw text on the screen.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element/Element.hpp>

namespace pu::element
{
    enum class TextAlign
    {
        CenterHorizontal,
        CenterVertical,
        CenterAll,
    };

    class TextBlock : public Element
    {
        public:
            TextBlock(u32 X, u32 Y, std::string Text, u32 FontSize = 25);
            ~TextBlock();
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            u32 GetHeight();
            std::string GetText();
            void SetText(std::string Text);
            void SetTextAlign(TextAlign Align);
            void RemoveTextAlign();
            bool HasTextAlign();
            void SetFont(render::NativeFont Font);
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch, bool Focus);
        private:
            bool halign;
            TextAlign talign;
            std::string text;
            u32 x;
            u32 y;
            render::NativeFont fnt;
            u32 fsize;
            draw::Color clr;
            render::NativeTexture ntex;
    };
}