#include "ui/framework/text_element.hpp"

#include "error.hpp"

namespace tin::ui
{
    TextElement::TextElement(u32 width, u32 height) :
        Element(width, height)
    {

    }

    void TextElement::Draw(Canvas canvas, Position position)
    {
        auto& fontRenderer = FontRenderer::Instance();
        fontRenderer.SetFontScale(m_fontFaceType, m_scale);
        fontRenderer.DrawText(m_fontFaceType, canvas, Position(position.x + m_leftInset, position.y + m_dimensions.height - m_bottomInset), m_colour, m_text);
    }

    void TextElement::SetText(const char* text)
    {
        size_t textLen = strlen(text);

        if (textLen >= 1024)
            return;

        memset(m_text, 0, 1024);
        strncpy(m_text, text, 1024-1);
    }

    void TextElement::SetFont(FontFaceType fontFaceType)
    {
        m_fontFaceType = fontFaceType;
    }

    void TextElement::SetColour(Colour colour)
    {
        m_colour = colour;
    }

    void TextElement::SetScale(unsigned int scale)
    {
        m_scale = scale;
    }

    void TextElement::SetInsets(unsigned int left, unsigned int bottom)
    {
        m_leftInset = left;
        m_bottomInset = bottom;
    }
}