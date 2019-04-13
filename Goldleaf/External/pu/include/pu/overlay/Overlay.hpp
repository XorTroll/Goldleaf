
/*

    Plutonium library

    @file Overlay.hpp
    @brief TODO...
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/Container.hpp>

namespace pu::overlay
{
    class Overlay : public Container
    {
        public:
            Overlay(u32 X, u32 Y, u32 Width, u32 Height, draw::Color Background, bool Rounded = true);
            ~Overlay();
            void SetRadius(u32 Radius);
            u32 GetRadius();
            virtual void OnPreRender(render::Renderer *Drawer);
            virtual void OnPostRender(render::Renderer *Drawer);
            bool Render(render::Renderer *Drawer);
            void NotifyEnding(bool End);
        private:
            s32 fadea;
            draw::Color bg;
            u32 rad;
            bool end;
            bool round;
    };
}