#pragma once

#include <switch.h>

namespace tin::ui
{
    struct ElementDimensions
    {
        unsigned int width;
        unsigned int height;

        ElementDimensions(unsigned int width, unsigned int height) :
            width(width), height(height)
        {

        }
    };

    struct ElementPosition
    {
        unsigned int x;
        unsigned int y;

        ElementPosition(unsigned int x, unsigned int y) :
            x(x), y(y)
        {

        }
    };

    class Element
    {
        protected:
            ElementDimensions m_dimensions;
            ElementPosition m_position;

            Element(u32 width, u32 height, u32 posX, u32 posY);

        public:
            virtual void DrawElement();

            ElementDimensions GetDimensions();
            ElementPosition GetPosition();
    };
}