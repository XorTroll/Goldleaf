#pragma once

#include "ui/framework/box_element.hpp"

namespace tin::ui
{
    class RowElement : public BoxElement
    {
        static const int DEFAULT_ROW_HEIGHT = 80;

        public:
            RowElement(u32 width, u32 height);
            RowElement(u32 width);
    };
}