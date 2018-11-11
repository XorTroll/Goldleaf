#pragma once

#include <array>
#include <switch.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ui/framework/canvas.hpp"
#include "ui/framework/layout.hpp"

namespace tin::ui
{
    enum FontFaceType : u8
    {
        STANDARD = 0x0,
        NINTY_EXT = 0x1,
        TOTAL
    };

    class FontRenderer final
    {
        private:
            FT_Library m_ftLibrary;
            std::array<FT_Face, FontFaceType::TOTAL> m_ftFaces;

            std::array<PlFontData, PlSharedFontType_Total> m_sharedFontData;
            size_t m_totalSharedFonts = 0;

        public:
            FontRenderer();
            ~FontRenderer();
            FontRenderer(FontRenderer const&) = delete;
            void operator=(FontRenderer const&)  = delete;

            static FontRenderer& Instance();

            FT_Error SetFontScale(FontFaceType type, u32 scale);
            void DrawChar(FontFaceType type, Canvas canvas, Position pos, Colour colour, const char c);
            void DrawText(FontFaceType type, Canvas canvas, Position pos, Colour colour, const char* text);
    };
}