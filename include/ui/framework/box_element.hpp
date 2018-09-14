#pragma once

#include "ui/framework/element.hpp"
#include "util/graphics_util.hpp"

namespace tin::ui
{
    class BoxElement : public Element
    {
        public:
            tin::util::Colour m_colour;

            BoxElement(u32 width, u32 height, u32 posX, u32 posY);

            virtual void DrawElement() override;

            void SetColour(tin::util::Colour colour);
    };
}