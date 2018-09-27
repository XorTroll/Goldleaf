#pragma once

#include <libpng16/png.h>
#include <switch.h>

#include "data/byte_buffer.hpp"
#include "ui/framework/canvas.hpp"
#include "ui/framework/layout.hpp"

namespace tin::ui
{
    enum class ImageType
    {
        PNG
    };

    class Image
    {
        private:
            tin::data::ByteBuffer m_rgbaBuffer;
            u32 m_width = 0;
            u32 m_height = 0;

            static void ReadPNGData(png_structp pngReadStructPtr, png_bytep bytesOut, png_size_t length);
            void ParsePNG(tin::data::ByteBuffer& bufIn);

        public:
            Image();
            Image(tin::data::ByteBuffer& buffer, ImageType type);

            void DrawImage(Canvas canvas, Position pos);
    };
}