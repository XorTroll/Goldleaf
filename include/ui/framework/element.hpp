#pragma once

#include <switch.h>
#include "ui/framework/layout.hpp"

namespace tin::ui
{
    class Element
    {
        protected:
            Dimensions m_dimensions;

            Element(u32 width, u32 height);

        public:
            virtual void DrawElement(Position position, Dimensions boundaries);

            Dimensions GetDimensions();
    };
}