#pragma once

#include "ui/framework/box_element.hpp"
#include "ui/framework/text_element.hpp"
#include "ui/framework/touch_handler.hpp"

namespace tin::ui
{
    class RowElement : public BoxElement
    {
        public:
            static const int ROW_HEIGHT = 80;

            BoxElement* m_underlineElement = nullptr;
            TextElement* m_textElement = nullptr;

            bool m_isSelected = false;

            RowElement(u32 width);

            void SetSelected(bool value);
    };

    class ListElement : public BoxElement
    {
        private:
            TouchHandler m_touchHandler;

            int m_scrollOffset = 0;
            int m_startScrollOffset = 0;

        public:
            ListElement(u32 width, u32 height);

            virtual void Draw(Canvas canvas, Position position) override;
            virtual void Update(Position position) override;
            
            void AddRow(const char* text);
    };
}