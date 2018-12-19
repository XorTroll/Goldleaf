
/*

    Plutonium library

    @file Dialog.hpp
    @brief A Dialog is an easy way to ask the user to choose between several options.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element/Image.hpp>
#include <vector>

namespace pu
{
    class Dialog
    {
        public:
            Dialog(std::string Title, std::string Content, draw::Font Font);
            void AddOption(std::string Name);
            void SetIcon(element::Image *Icon);
            bool Hasicon();
            void Show(render::Renderer *Drawer);
            bool UserCancelled();
            u32 GetSelectedIndex();
        private:
            std::string title;
            std::string cnt;
            std::vector<std::string> opts;
            u32 osel;
            bool cancel;
            bool hicon;
            element::Image *icon;
            u32 prevosel;
            s32 pselfact;
            s32 selfact;
            draw::Font fnt;
    };
}