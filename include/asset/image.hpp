#pragma once

#include <libpng16/png.h>
#include <switch.h>

#include "data/byte_buffer.hpp"
#include "ui/framework/canvas.hpp"
#include "ui/framework/layout.hpp"

namespace tin::asset
{
    enum class ImageType
    {
        PNG
    };

    class Image
    {
        private:
            tin::data::ByteBuffer m_abgrBuffer;
            tin::ui::Dimensions m_dimensions;

            static void ReadPNGData(png_structp pngReadStructPtr, png_bytep bytesOut, png_size_t length);
            void ParsePNG(tin::data::ByteBuffer& bufIn);

        public:
            Image();
            Image(tin::data::ByteBuffer& buffer, ImageType type);

            void DrawImage(tin::ui::Canvas canvas, tin::ui::Position pos);
            void ScaleImage(tin::ui::Dimensions scaledDimensions);
    };
}