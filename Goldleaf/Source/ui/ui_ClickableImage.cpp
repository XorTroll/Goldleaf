
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

#include <ui/ui_ClickableImage.hpp>

namespace ui {

    ClickableImage::ClickableImage(s32 x, s32 y, const std::string &img) : pu::ui::elm::Element::Element(), x(x), y(y), w(0), h(0), native_tex(nullptr), cb([&](){}), touched(false) {
        this->SetImage(img);
    }

    ClickableImage::~ClickableImage() {
        if(this->native_tex != nullptr) {
            pu::ui::render::DeleteTexture(this->native_tex);
            this->native_tex = nullptr;
        }
    }

    s32 ClickableImage::GetX() {
        return this->x;
    }

    void ClickableImage::SetX(s32 x) {
        this->x = x;
    }

    s32 ClickableImage::GetY() {
        return this->y;
    }

    void ClickableImage::SetY(s32 y) {
        this->y = y;
    }

    s32 ClickableImage::GetWidth() {
        return this->w;
    }

    void ClickableImage::SetWidth(s32 w) {
        this->w = w;
    }

    s32 ClickableImage::GetHeight() {
        return this->h;
    }

    void ClickableImage::SetHeight(s32 h) {
        this->h = h;
    }

    std::string ClickableImage::GetImage() {
        return this->img;
    }

    void ClickableImage::SetImage(const std::string &img) {
        if(this->native_tex != nullptr) {
            pu::ui::render::DeleteTexture(this->native_tex);
        }
        this->native_tex = nullptr;
        auto exp = fs::GetExplorerForPath(img);
        if(exp->IsFile(img)) {
            this->img = img;
            this->native_tex = pu::ui::render::LoadImage(img);
            this->w = pu::ui::render::GetTextureWidth(this->native_tex);
            this->h = pu::ui::render::GetTextureHeight(this->native_tex);
        }
    }

    bool ClickableImage::IsImageValid() {
        return (this->native_tex != nullptr) && !this->img.empty();
    }

    void ClickableImage::SetOnClick(std::function<void()> cb_fn) {
        this->cb = cb_fn;
    }

    void ClickableImage::OnRender(pu::ui::render::Renderer::Ref &drawer, s32 x, s32 y) {
        drawer->RenderTexture(this->native_tex, x, y, { -1, w, h, -1 });
    }

    void ClickableImage::OnInput(u64 down, u64 up, u64 held, pu::ui::Touch pos) {
        if(this->touched) {
            const auto time_now = std::chrono::steady_clock::now();
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - this->touch_time_point).count();
            if(diff >= 200) {
                this->touched = false;
                (this->cb)();
                SDL_SetTextureColorMod(this->native_tex, 0xFF, 0xFF, 0xFF);
            }
        }
        else if(!pos.IsEmpty()) {
            touchPosition tch = {};
            hidTouchRead(&tch, 0);
            if((pos.X >= this->GetProcessedX()) && (pos.X < (this->GetProcessedX() + w)) && (pos.Y >= this->GetProcessedY()) && (pos.Y < (this->GetProcessedY() + h))) {
                this->touch_time_point = std::chrono::steady_clock::now();
                this->touched = true;
                SDL_SetTextureColorMod(this->native_tex, 200, 200, 0xFF);
            }
        }
    }

}