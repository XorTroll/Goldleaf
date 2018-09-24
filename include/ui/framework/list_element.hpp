#pragma once

#include "ui/framework/box_element.hpp"
#include "ui/framework/text_element.hpp"
#include "ui/framework/touch_handler.hpp"

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
        private:
            TouchHandler m_touchHandler;

        public:
            ListElement(u32 width, u32 height);

            virtual void Draw(Canvas canvas, Position position) override;
            virtual void Update(Position position) override;
            
            void AddRow(const char* text);
    };
}