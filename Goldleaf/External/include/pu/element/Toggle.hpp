
#pragma once
#include <pu/element/Element.hpp>

namespace pu::element
{
    class Toggle : public Element
    {
        public:
            Toggle(u32 X, u32 Y, std::string Content, u64 Key, draw::Color Checked);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            u32 GetHeight();
            std::string GetContent();
            void SetContent(std::string Content);
            draw::Font GetFont();
            void SetFont(draw::Font Font);
            u32 GetFontSize();
            void SetFontSize(u32 Size);
            draw::Color GetColor();
            void SetColor(draw::Color General);
            u64 GetKey();
            void SetKey(u64 Key);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Input);
            bool IsChecked();
        private:
            std::string cnt;
            u32 x;
            u32 y;
            u64 key;
            bool checked;
            draw::Color clr;
            draw::Font fnt;
            u32 fsize;
            s32 togfact;
    };
}