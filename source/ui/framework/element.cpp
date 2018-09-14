#include "ui/framework/element.hpp"

#include "error.hpp"

namespace tin::ui
{
    Element::Element(u32 width, u32 height, u32 posX, u32 posY) :
        m_dimensions(width, height), m_position(posX, posY)
    {

    }

    void Element::DrawElement() 
    {
        LOG_DEBUG("Base DrawElement called\n");
    }

    ElementDimensions Element::GetDimensions()
    {
        return m_dimensions;
    }

    ElementPosition Element::GetPosition()
    {
        return m_position;
    }
}