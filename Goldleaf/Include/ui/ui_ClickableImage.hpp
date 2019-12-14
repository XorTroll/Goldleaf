
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once
#include <ui/ui_Includes.hpp>
#include <pu/Plutonium>

namespace ui
{
    class ClickableImage : public pu::ui::elm::Element
    {
        public:
            ClickableImage(s32 X, s32 Y, String Image);
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
            void OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos);
        protected:
            String img;
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