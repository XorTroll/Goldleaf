
/*

    Plutonium library

    @file Dialog.hpp
    @brief A Dialog is an easy way to ask the user to choose between several options.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/render.hpp>
#include <vector>

namespace pu
{
    class Dialog
    {
        public:
            Dialog(std::string Title, std::string Content);
            ~Dialog();
            void AddOption(std::string Name);
            void SetIcon(std::string Icon, u32 IconX, u32 IconY);
            bool Hasicon();
            void Show(render::Renderer *Drawer);
            bool UserCancelled();
            u32 GetSelectedIndex();
        private:
            render::NativeFont tfont;
            render::NativeFont cfont;
            render::NativeFont ofont;
            render::NativeTexture title;
            render::NativeTexture cnt;
            std::vector<std::string> sopts;
            std::vector<render::NativeTexture> opts;
            u32 osel;
            bool cancel;
            bool hicon;
            render::NativeTexture icon;
            u32 ix;
            u32 iy;
            u32 prevosel;
            s32 pselfact;
            s32 selfact;
    };
}