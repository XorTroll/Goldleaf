
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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
        public:
            using OnClickCallback = std::function<void()>;

        protected:
            s32 x;
            s32 y;
            s32 w;
            s32 h;
            std::string img;
            pu::sdl2::Texture img_tex;
            OnClickCallback cb;
            std::chrono::steady_clock::time_point touch_time_point;
            bool touched;

        public:
            ClickableImage(const s32 x, const s32 y, const std::string &img);
            PU_SMART_CTOR(ClickableImage)
            ~ClickableImage();

            inline s32 GetX() override {
                return this->x;
            }

            inline void SetX(const s32 x) {
                this->x = x;
            }

            inline s32 GetY() override {
                return this->y;
            }

            inline void SetY(const s32 y) {
                this->y = y;
            }

            inline s32 GetWidth() override {
                return this->w;
            }

            inline void SetWidth(const s32 w) {
                this->w = w;
            }

            inline s32 GetHeight() override {
                return this->h;
            }

            inline void SetHeight(const s32 h) {
                this->h = h;
            }

            inline std::string GetImage() {
                return this->img;
            }
            
            void SetImage(const std::string &img);
            
            inline bool IsImageValid() {
                return this->img_tex != nullptr;
            }
            
            inline void SetOnClick(OnClickCallback cb) {
                this->cb = cb;
            }

            void OnRender(pu::ui::render::Renderer::Ref &drawer, const s32 x, const s32 y) override;
            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint pos) override;
    };

}