#pragma once

#include "ui/framework/element.hpp"
#include "ui/framework/font_renderer.hpp"
#include "ui/framework/canvas.hpp"

namespace tin::ui
{
    class TextElement : public Element
    {
        private:
            char m_text[1024] = {0};
            FontFaceType m_fontFaceType = FontFaceType::NINTY_EXT;
            Colour m_colour = Colour(0xFF, 0xFF, 0xFF, 0xFF);
            unsigned int m_scale = 5;

            unsigned int m_leftInset = 0;
            unsigned int m_bottomInset = 0;

        public:
            TextElement(u32 width, u32 height);

            virtual void Draw(Canvas canvas, Position position) override;

            void SetText(const char* text);
            void SetFont(FontFaceType fontFaceType);
            void SetColour(Colour colour);
            void SetScale(unsigned int scale);
            void SetInsets(unsigned int left, unsigned int bottom);
    };
}