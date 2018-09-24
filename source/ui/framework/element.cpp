#include "ui/framework/element.hpp"

#include "error.hpp"

namespace tin::ui
{
    Element::Element(u32 width, u32 height) :
        m_dimensions(width, height)
    {
        if (m_dimensions.width == 0 || m_dimensions.height == 0)
            THROW_FORMAT("Both dimensions must be non-zero! %u x %u\n", m_dimensions.width, m_dimensions.height);
    }

    void Element::Draw(Canvas canvas, Position position) 
    {
        //LOG_DEBUG("Base Draw called\n");
    }

    void Element::Update(Position position)
    {
        //LOG_DEBUG("Base Update called\n");
    }

    Dimensions Element::GetDimensions()
    {
        return m_dimensions;
    }
}