
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

    ClickableImage::ClickableImage(const s32 x, const s32 y, const std::string &img) : pu::ui::elm::Element::Element(), x(x), y(y), w(0), h(0), img_tex(nullptr), cb({}), touched(false) {
        this->SetImage(img);
    }

    ClickableImage::~ClickableImage() {
        pu::ui::render::DeleteTexture(this->img_tex);
    }

    void ClickableImage::SetImage(const std::string &img) {
        pu::ui::render::DeleteTexture(this->img_tex);
        
        auto exp = fs::GetExplorerForPath(img);
        if(exp->IsFile(img)) {
            this->img = img;
            this->img_tex = pu::ui::render::LoadImage(img);
            this->w = pu::ui::render::GetTextureWidth(this->img_tex);
            this->h = pu::ui::render::GetTextureHeight(this->img_tex);
        }
    }

    void ClickableImage::OnRender(pu::ui::render::Renderer::Ref &drawer, const s32 x, const s32 y) {
        drawer->RenderTexture(this->img_tex, x, y, pu::ui::render::TextureRenderOptions::WithCustomDimensions(this->w, this->h));
    }

    void ClickableImage::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint pos) {
        if(this->touched) {
            const auto time_now = std::chrono::steady_clock::now();
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - this->touch_time_point).count();
            if(diff >= 200) {
                this->touched = false;
                if(this->cb) {
                    (this->cb)();
                }
                SDL_SetTextureColorMod(this->img_tex, 0xFF, 0xFF, 0xFF);
            }
        }
        else if(pos.HitsRegion(this->GetProcessedX(), this->GetProcessedY(), this->w, this->h)) {
            this->touch_time_point = std::chrono::steady_clock::now();
            this->touched = true;
            SDL_SetTextureColorMod(this->img_tex, 200, 200, 0xFF);
        }
    }

}