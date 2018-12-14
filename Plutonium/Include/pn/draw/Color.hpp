
#pragma once
#include <switch.h>

namespace pn::draw
{
    struct Color
    {
        Color();
        Color(u8 R, u8 G, u8 B, u8 A);
        u8 R;
        u8 G;
        u8 B;
        u8 A;
    };
}