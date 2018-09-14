#include "ui/framework/element.hpp"

#include "error.hpp"

namespace tin::ui
{
    Element::Element(u32 width, u32 height) :
        m_dimensions(width, height)
    {

    }

    void Element::DrawElement(Position position) 
    {
        LOG_DEBUG("Base DrawElement called\n");
    }

    Dimensions Element::GetDimensions()
    {
        return m_dimensions;
    }
}