
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

#include <ui/ui_ClickableImage.hpp>
#include <fs/fs_Common.hpp>

namespace ui
{
    ClickableImage::ClickableImage(s32 X, s32 Y, String Image) : pu::ui::elm::Element::Element()
    {
        this->x = X;
        this->y = Y;
        this->w = 0;
        this->h = 0;
        this->ntex = NULL;
        this->touched = false;
        this->cb = [&](){};
        this->SetImage(Image);
    }

    ClickableImage::~ClickableImage()
    {
        if(this->ntex != NULL)
        {
            pu::ui::render::DeleteTexture(this->ntex);
            this->ntex = NULL;
        }
    }

    s32 ClickableImage::GetX()
    {
        return this->x;
    }

    void ClickableImage::SetX(s32 X)
    {
        this->x = X;
    }

    s32 ClickableImage::GetY()
    {
        return this->y;
    }

    void ClickableImage::SetY(s32 Y)
    {
        this->y = Y;
    }

    s32 ClickableImage::GetWidth()
    {
        return this->w;
    }

    void ClickableImage::SetWidth(s32 Width)
    {
        this->w = Width;
    }

    s32 ClickableImage::GetHeight()
    {
        return this->h;
    }

    void ClickableImage::SetHeight(s32 Height)
    {
        this->h = Height;
    }

    String ClickableImage::GetImage()
    {
        return this->img;
    }

    void ClickableImage::SetImage(String Image)
    {
        if(this->ntex != NULL) pu::ui::render::DeleteTexture(this->ntex);
        this->ntex = NULL;
        if(fs::IsFile(Image))
        {
            this->img = Image;
            this->ntex = pu::ui::render::LoadImage(Image.AsUTF8());
            this->w = pu::ui::render::GetTextureWidth(this->ntex);
            this->h = pu::ui::render::GetTextureHeight(this->ntex);
        }
    }

    bool ClickableImage::IsImageValid()
    {
        return ((ntex != NULL) && this->img.HasAny());
    }

    void ClickableImage::SetOnClick(std::function<void()> Callback)
    {
        cb = Callback;
    }

    void ClickableImage::OnRender(pu::ui::render::Renderer::Ref &Drawer, s32 X, s32 Y)
    {
        Drawer->RenderTexture(this->ntex, X, Y, { -1, w, h, -1 });
    }

    void ClickableImage::OnInput(u64 Down, u64 Up, u64 Held, pu::ui::Touch Pos)
    {
        if(touched)
        {
            auto tpnow = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(tpnow - touchtp).count();
            if(diff >= 200)
            {
                touched = false;
                (this->cb)();
                SDL_SetTextureColorMod(ntex, 255, 255, 255);
            }
        }
        else if(!Pos.IsEmpty())
        {
            touchPosition tch;
            hidTouchRead(&tch, 0);
            if((Pos.X >= this->GetProcessedX()) && (Pos.X < (this->GetProcessedX() + w)) && (Pos.Y >= this->GetProcessedY()) && (Pos.Y < (this->GetProcessedY() + h)))
            {
                touchtp = std::chrono::steady_clock::now();
                touched = true;
                SDL_SetTextureColorMod(ntex, 200, 200, 255);
            }
        }
    }
}