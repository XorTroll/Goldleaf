#include "ui/framework/box_element.hpp"

#include "util/graphics_util.hpp"
#include "error.hpp"

namespace tin::ui
{
    BoxElement::BoxElement(u32 width, u32 height) :
        Element(width, height)
    {

    }

    void BoxElement::DrawElement(Position position, Dimensions boundaries)
    {
        unsigned int renderHeight = m_dimensions.height == 0 ? boundaries.height : m_dimensions.height;
        unsigned int renderWidth = m_dimensions.width == 0 ? boundaries.width : m_dimensions.width;

        for (u32 y = 0; y < renderHeight; y++)
        {
            for (u32 x = 0; x < renderWidth; x++)
            {
                u32 pixelX = x + position.x;
                u32 pixelY = y + position.y;

                tin::util::DrawPixel(pixelX, pixelY, m_colour);
            }
        }

        // No need to draw sub elements if we don't have any
        if (m_subElements.empty())
            return;

        unsigned int startOffset = 0;
        Dimensions subElementBoundaries(m_dimensions.width - m_subElementLayout.leftInset - m_subElementLayout.rightInset, m_dimensions.height - m_subElementLayout.topInset - m_subElementLayout.bottomInset);
        
        for (auto& subElement : m_subElements)
        {
            unsigned int startX = position.x + m_subElementLayout.leftInset;
            unsigned int startY = position.y + m_subElementLayout.topInset;

            switch (m_subElementLayout.arrangementType)
            {
                case SubElementArrangementType::TOP_TO_BOTTOM:
                    startY += startOffset;
                    break;

                default:
                    startX += startOffset;
                    break;
            }

            subElement->DrawElement(Position(startX, startY), subElementBoundaries);
            startOffset += subElement->GetDimensions().width + m_subElementLayout.gapSize;
        }
    }

    void BoxElement::SetColour(tin::util::Colour colour)
    {
        m_colour = colour;
    }

    void BoxElement::SetSubElementLayout(SubElementLayout subElementLayout)
    {
        m_subElementLayout = subElementLayout;
    }

    void BoxElement::AddSubElement(std::unique_ptr<Element> element)
    {
        m_subElements.push_back(std::move(element));
    }
}