#pragma once

#include <string>
#include <switch.h>

extern u8* g_framebuf;
extern u32 g_framebufWidth;
extern u32 g_framebufHeight;

namespace tin::util
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

    void PrintTextCentred(std::string text);

    // TODO: Remove other references to the framebuf, just use this instead
    inline void DrawPixel(u32 x, u32 y, Colour colour)
    {
        if (x >= 1280 || y >= 720)
            return;
        
        u32 off = (y * g_framebufWidth + x) * 4;
        *((u32*)&g_framebuf[off]) = colour.r | (colour.g << 8) | (colour.b << 16) | (0xff << 24);
    }
}