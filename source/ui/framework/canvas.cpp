#include "ui/framework/canvas.hpp"

namespace tin::ui
{
    Canvas::Canvas() :
       m_restrictionPosition(0, 0), m_restrictionDimensions(1280, 720)
    {

    }

    Canvas::Canvas(Position restrictionPosition, Dimensions restrictionDimensions) :
        m_restrictionPosition(restrictionPosition), m_restrictionDimensions(restrictionDimensions)
    {
        
    }

    void Canvas::DrawPixel(u32 x, u32 y, Colour colour, bool restricted)
    {
        if (x >= 1280 || y >= 720)
            return;

        if (restricted && (x < m_restrictionPosition.x || x >= (m_restrictionPosition.x + m_restrictionDimensions.width) || y < m_restrictionPosition.y || y >= ((m_restrictionPosition.y + m_restrictionDimensions.height))))
            return;

        u32 off = (y * g_framebufWidth + x) * 4;
        *((u32*)&g_framebuf[off]) = colour.r | (colour.g << 8) | (colour.b << 16) | (0xff << 24);
    }
}