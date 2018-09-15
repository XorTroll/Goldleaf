#pragma once

#include <switch.h>
#include "ui/framework/layout.hpp"

extern u8* g_framebuf;
extern u32 g_framebufWidth;
extern u32 g_framebufHeight;

namespace tin::ui
{
    union Colour
    {
        u32 abgr;
        struct
        {
            u8 r, g, b, a;
        };

        Colour(u8 r, u8 g, u8 b, u8 a) :
            r(r), g(g), b(b), a(a)
        {

        }

        Colour(u32 rgb) :
            r(rgb >> 16), g(rgb >> 8), b(rgb), a(0xFF)
        {

        } 

        Colour() :
            Colour(0xFFFFFF)
        {}
    };

    class Canvas
    {
        public:
            const Position m_restrictionPosition;
            const Dimensions m_restrictionDimensions; 
            
            Canvas();
            Canvas(Position restrictionPosition, Dimensions restrictionDimensions);

            void DrawPixel(u32 x, u32 y, Colour colour, bool restricted=true);
    };
}