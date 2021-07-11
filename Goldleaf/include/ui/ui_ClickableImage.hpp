
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

namespace ui {

    class ClickableImage : public pu::ui::elm::Element {
        protected:
            s32 x;
            s32 y;
            s32 w;
            s32 h;
            std::string img;
            pu::sdl2::Texture native_tex;
            std::function<void()> cb;
            std::chrono::steady_clock::time_point touch_time_point;
            bool touched;
        public:
            ClickableImage(s32 x, s32 y, const std::string &img);
            PU_SMART_CTOR(ClickableImage)
            ~ClickableImage();

            s32 GetX();
            void SetX(s32 x);
            s32 GetY();
            void SetY(s32 y);
            s32 GetWidth();
            void SetWidth(s32 w);
            s32 GetHeight();
            void SetHeight(s32 h);
            std::string GetImage();
            void SetImage(const std::string &img);
            bool IsImageValid();
            void SetOnClick(std::function<void()> cb_fn);
            void OnRender(pu::ui::render::Renderer::Ref &drawer, s32 x, s32 y);
            void OnInput(u64 down, u64 up, u64 held, pu::ui::Touch pos);
    };

}