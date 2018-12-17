
#pragma once
#include <functional>
#include <pu/element/Element.hpp>

namespace pu::element
{
    class Button : public Element
    {
        public:
            Button(u32 X, u32 Y, u32 Width, u32 Height, std::string Content, draw::Color Color);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            void SetWidth(u32 Width);
            u32 GetHeight();
            void SetHeight(u32 Height);
            std::string GetContent();
            void SetContent(std::string Content);
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            draw::Font GetContentFont();
            void SetContentFont(draw::Font Font);
            void SetOnClick(std::function<void()> ClickCallback);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Input);
        private:
            u32 x;
            u32 y;
            u32 w;
            u32 h;
            draw::Font fnt;
            u32 fsize;
            draw::Color clr;
            std::string cnt;
            std::function<void()> clickcb;
            bool hover;
            s32 hoverfact;
    };
}