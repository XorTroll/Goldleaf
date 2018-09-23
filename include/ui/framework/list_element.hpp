#pragma once

#include "ui/framework/box_element.hpp"
#include "ui/framework/text_element.hpp"

namespace tin::ui
{
    class RowElement : public BoxElement
    {
        static const int DEFAULT_ROW_HEIGHT = 80;

        public:
            TextElement* m_textElement = nullptr;

            RowElement(u32 width, u32 height);
            RowElement(u32 width);
    };

    class ListElement : public BoxElement
    {
        public:
            ListElement(u32 width, u32 height);

            void AddRow(const char* text);
    };
}