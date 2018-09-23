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

            static inline uint8_t BlendColour(uint32_t src, uint32_t dst, uint8_t alpha)
            {
                uint8_t one_minus_alpha = (uint8_t)255 - alpha;
                return (dst*alpha + src*one_minus_alpha)/(uint8_t)255;
            }

            void DrawPixel(u32 x, u32 y, Colour colour, bool restricted=true);
            void DrawPixelBlend(u32 x, u32 y, Colour colour, bool restricted=true);

            Canvas Intersect(Position pos, Dimensions dimensions);
    };
}