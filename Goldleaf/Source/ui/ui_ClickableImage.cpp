#include <ui/ui_ClickableImage.hpp>

namespace ui
{
    ClickableImage::ClickableImage(s32 X, s32 Y, pu::String Image) : pu::ui::elm::Element::Element()
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

    pu::String ClickableImage::GetImage()
    {
        return this->img;
    }

    void ClickableImage::SetImage(pu::String Image)
    {
        if(this->ntex != NULL) pu::ui::render::DeleteTexture(this->ntex);
        this->ntex = NULL;
        std::ifstream ifs(Image.AsUTF8());
        if(ifs.good())
        {
            ifs.close();
            this->img = Image;
            this->ntex = pu::ui::render::LoadImage(Image.AsUTF8());
            this->w = pu::ui::render::GetTextureWidth(this->ntex);
            this->h = pu::ui::render::GetTextureHeight(this->ntex);
        }
        ifs.close();
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
        s32 rdx = X;
        s32 rdy = Y;
        Drawer->RenderTextureScaled(this->ntex, rdx, rdy, w, h);
    }

    void ClickableImage::OnInput(u64 Down, u64 Up, u64 Held, bool Touch)
    {
        if(touched)
        {
            auto tpnow = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(tpnow - touchtp).count();
            if(diff >= 200)
            {
                (this->cb)();
                touched = false;
                SDL_SetTextureColorMod(ntex, 255, 255, 255);
            }
        }
        else if(Touch)
        {
            touchPosition tch;
            hidTouchRead(&tch, 0);
            if(((s32)tch.px >= x) && ((s32)tch.px < (x + w)) && ((s32)tch.py >= y) && ((s32)tch.py < (y + h)))
            {
                touchtp = std::chrono::steady_clock::now();
                touched = true;
                SDL_SetTextureColorMod(ntex, 200, 200, 255);
            }
        }
    }
}