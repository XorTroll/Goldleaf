#pragma once

#include <vector>
#include "ui/framework/element.hpp"
#include "util/graphics_util.hpp"

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
    };

    class BoxElement : public Element
    {
        public:
            tin::util::Colour m_colour;
            SubElementLayout m_subElementLayout;

            BoxElement(u32 width, u32 height);

            virtual void DrawElement(Position position) override;

            void SetColour(tin::util::Colour colour);
    };
}