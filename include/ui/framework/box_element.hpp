#pragma once

#include <memory>
#include <vector>
#include "ui/framework/element.hpp"
#include "ui/framework/canvas.hpp"

namespace tin::ui
{
    enum class SubElementArrangementType
    {
        LEFT_TO_RIGHT, TOP_TO_BOTTOM
    };

    struct SubElementLayout
    {
        public:
            SubElementArrangementType arrangementType = SubElementArrangementType::LEFT_TO_RIGHT;
            unsigned int gapSize = 0;

            // Offset subelements from the outer edges of the parent box
            unsigned int leftInset = 0;
            unsigned int rightInset = 0;
            unsigned int topInset = 0;
            unsigned int bottomInset = 0;
    };

    class BoxElement : public Element
    {
        public:
            tin::ui::Colour m_colour;
            SubElementLayout m_subElementLayout;
            std::vector<std::unique_ptr<Element>> m_subElements;

            BoxElement(u32 width, u32 height);

            virtual void Draw(Canvas canvas, Position position) override;

            void SetColour(tin::ui::Colour colour);
            void SetSubElementLayout(SubElementLayout subElementLayout);

            void AddSubElement(std::unique_ptr<Element> element);
    };
}