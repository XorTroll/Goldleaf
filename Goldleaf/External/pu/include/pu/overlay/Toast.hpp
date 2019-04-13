
/*

    Plutonium library

    @file Overlay.hpp
    @brief TODO...
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/overlay/Overlay.hpp>

namespace pu::overlay
{
    class Toast : public Overlay
    {
        public:
            Toast(std::string Text, u32 FontSize, draw::Color TextColor, draw::Color BaseColor);
            void SetText(std::string Text);
            void OnPreRender(render::Renderer *Drawer);
            void OnPostRender(render::Renderer *Drawer);
        private:
            pu::element::TextBlock *text;
    };
}