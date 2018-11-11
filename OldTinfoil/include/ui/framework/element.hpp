#pragma once

#include <switch.h>
#include "ui/framework/layout.hpp"
#include "ui/framework/canvas.hpp"

namespace tin::ui
{
    class Element
    {
        protected:
            Dimensions m_dimensions;

            Element(u32 width, u32 height);

        public:
            virtual void Draw(Canvas canvas, Position position);
            virtual void Update(Position position);

            Dimensions GetDimensions();
    };
}