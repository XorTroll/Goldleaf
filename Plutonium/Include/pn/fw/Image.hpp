
#pragma once
#include <pn/fw/Element.hpp>

namespace pn::fw
{
    class Image : public Element
    {
        public:
            Image(u32 X, u32 Y, std::string Image);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            void SetWidth(u32 Width);
            u32 GetHeight();
            void SetHeight(u32 Height);
            std::string GetImage();
            void SetImage(std::string Image);
            bool IsImageValid();
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Input);
        private:
            std::string img;
            u32 x;
            u32 y;
            u32 w;
            u32 h;
    };
}