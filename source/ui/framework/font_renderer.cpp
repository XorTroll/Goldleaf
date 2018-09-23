#include "ui/framework/font_renderer.hpp"

#include "error.hpp"

namespace tin::ui
{
    FontRenderer::FontRenderer()
    {
        u64 languageCode = 0;

        ASSERT_OK(setGetSystemLanguage(&languageCode), "Failed to get system language code\n");
        ASSERT_OK(plGetSharedFont(languageCode, m_sharedFontData.data(), PlSharedFontType_Total, &m_totalSharedFonts), "Failed to get shared fonts");
        
        if (m_totalSharedFonts == 0)
            THROW_FORMAT("No shared fonts found!\n");
        
        ASSERT_OK(FT_Init_FreeType(&m_ftLibrary), "Failed to init freetype");

        PlFontData standardFontData = m_sharedFontData[PlSharedFontType_Standard];
        ASSERT_OK(FT_New_Memory_Face(m_ftLibrary, (const FT_Byte*)standardFontData.address, standardFontData.size, 0, &m_ftFaces[FontFaceType::STANDARD]), "Failed to create standard font face");
    
        PlFontData extendedFontData = m_sharedFontData[PlSharedFontType_NintendoExt];
        ASSERT_OK(FT_New_Memory_Face(m_ftLibrary, (const FT_Byte*)extendedFontData.address, extendedFontData.size, 0, &m_ftFaces[FontFaceType::NINTY_EXT]), "Failed to create extended font face");
    }

    FontRenderer::~FontRenderer()
    {
        for (auto& face : m_ftFaces)
        {
            FT_Done_Face(face);
        }

        FT_Done_FreeType(m_ftLibrary);
    }

    FontRenderer& FontRenderer::Instance()
    {
        static FontRenderer instance;
        return instance;
    }

     FT_Error FontRenderer::SetFontScale(FontFaceType type, u32 scale)
     {
        return FT_Set_Char_Size(m_ftFaces[type], 0, scale * 64, 300, 300);
     }

    void FontRenderer::DrawChar(FontFaceType type, Canvas canvas, Position pos, Colour colour, const char c)
    {
        FT_Face face = m_ftFaces[type];
        u32 utf8Char = 0;
        ssize_t unitCount = decode_utf8(&utf8Char, (const uint8_t*)&c);

        if (unitCount <= 0)
            return;

        FT_UInt glyphIndex = FT_Get_Char_Index(face, utf8Char);

        if (R_FAILED(FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)))
            return;

        if (R_FAILED(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)))
            return;
            
        FT_GlyphSlot slot = face->glyph;
        u32 x = pos.x + slot->bitmap_left;
        u32 y = pos.y - slot->bitmap_top;
        FT_Bitmap* bitmap = &slot->bitmap;
        u8* imageptr = bitmap->buffer;

        if (bitmap->pixel_mode != FT_PIXEL_MODE_GRAY) 
            return;

        for (u32 tmpy = 0; tmpy < bitmap->rows; tmpy++)
        {
            for (u32 tmpx = 0; tmpx < bitmap->width; tmpx++)
            {
                colour.a = imageptr[tmpx];

                // Skip blanks
                if (!colour.a)
                    continue;

                canvas.DrawPixelBlend(x + tmpx, y + tmpy, colour);
            }

            imageptr += bitmap->pitch;
        }
    }

    void FontRenderer::DrawText(FontFaceType type, Canvas canvas, Position pos, Colour colour, const char* text)
    {
        u32 x = pos.x;
        u32 y = pos.y;
        FT_Face face = m_ftFaces[type];
        FT_GlyphSlot slot = face->glyph;

        u32 textLen = strlen(text);

        for (u32 i = 0; i < textLen; i++)
        {
            char c = text[i];

            if (c == '\n')
            {
                x = pos.x;
                y += face->size->metrics.height / 64;
                continue;
            }

            this->DrawChar(type, canvas, Position(x, y), colour, c);

            x += slot->advance.x >> 6;
            y += slot->advance.y >> 6;
        }
    }
}