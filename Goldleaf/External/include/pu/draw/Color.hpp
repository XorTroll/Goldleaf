
/*

    Plutonium library

    @file Color.hpp
    @brief A Color is used for basic RGBA drawing in all kind of elements.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <switch.h>

namespace pu::draw
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