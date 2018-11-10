#pragma once

namespace tin::ui
{
    struct Dimensions
    {
        unsigned int width = 0;
        unsigned int height = 0;

        Dimensions();
        Dimensions(unsigned int width, unsigned int height) :
            width(width), height(height)
        {

        }
    };

    struct Position
    {
        unsigned int x = 0;
        unsigned int y = 0;

        Position();
        Position(unsigned int x, unsigned int y) :
            x(x), y(y)
        {

        }
    };
}