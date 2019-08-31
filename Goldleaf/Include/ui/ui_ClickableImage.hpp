
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
    class ClickableImage : public pu::ui::elm::Element
    {
        public:
            ClickableImage(s32 X, s32 Y, pu::String Image);
            PU_SMART_CTOR(ClickableImage)
            ~ClickableImage();

            s32 GetX();
            void SetX(s32 X);
            s32 GetY();
            void SetY(s32 Y);
            s32 GetWidth();
            void SetWidth(s32 Width);
            s32 GetHeight();
            void SetHeight(s32 Height);
            pu::String GetImage();
            void SetImage(pu::String Image);
            bool IsImageValid();
            void SetOnClick(std::function<void()> Callback);
            void OnRender(pu::ui::render::Renderer::Ref &Drawer, s32 X, s32 Y);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch);
        protected:
            pu::String img;
            pu::ui::render::NativeTexture ntex;
            s32 x;
            s32 y;
            s32 w;
            s32 h;
            std::function<void()> cb;
            std::chrono::steady_clock::time_point touchtp;
            bool touched;
    };
}