
/*

    Plutonium library

    @file Button.hpp
    @brief A Button is an Element for option selecting.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <functional>
#include <pu/element/Element.hpp>

namespace pu::element
{
    class IconButton : public Element
    {
        public:
            IconButton(u32 X, u32 Y, u32 Radius, std::string Icon);
            ~IconButton();
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            u32 GetHeight();
            u32 GetRadius();
            void SetRadius(u32 Radius);
            std::string GetIconPath();
            void SetIconPath(std::string Icon);
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            void SetOnClick(std::function<void()> ClickCallback);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch);
        private:
            u32 x;
            u32 y;
            u32 rad;
            draw::Color clr;
            std::string icon;
            std::function<void()> clickcb;
            bool doclick;
            bool hover;
            s32 hoverfact;
            render::NativeTexture ntex;
    };
}