#include "asset/image.hpp"

#include "data/byte_stream.hpp"
#include "error.hpp"

namespace tin::asset
{
    Image::Image() :
        m_dimensions(0, 0)
    {
        
    }

    Image::Image(tin::data::ByteBuffer& buffer, ImageType type) :
        m_dimensions(0, 0)
    {
        switch (type)
        {
            case ImageType::PNG:
                this->ParsePNG(buffer);
                break;
        }
    }

    void Image::ReadPNGData(png_structp pngReadStructPtr, png_bytep bytesOut, png_size_t length)
    {
        png_voidp ioPtr = png_get_io_ptr(pngReadStructPtr);

        if (ioPtr == NULL)
            return;

        tin::data::BufferedByteStream* stream = (tin::data::BufferedByteStream*)ioPtr;
        stream->ReadBytes(bytesOut, length);
    }

    void Image::ParsePNG(tin::data::ByteBuffer& bufIn)
    {
        // Check if this is a valid png
        bool validPNGSig = !png_sig_cmp(bufIn.GetData(), 0, 8);

        if (!validPNGSig)
        {
            LOG_DEBUG("Invalid PNG Sig!\n");
            return;
        }

        LOG_DEBUG("Valid PNG sig!\n");
    
        // Create the struct for reading the png
        png_structp pngReadStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
        if (!pngReadStructPtr)
        {
            LOG_DEBUG("Failed to create PNG read struct!\n");
            return;
        }

        // Create the struct which holds png info
        png_infop pngInfoStructPtr = png_create_info_struct(pngReadStructPtr);

        if (!pngInfoStructPtr)
        {
            LOG_DEBUG("Failed to create PNG info struct!\n");
            png_destroy_read_struct(&pngReadStructPtr, NULL, NULL);
            return;
        }

        tin::data::BufferedByteStream bufferedByteStream(bufIn);

        png_set_read_fn(pngReadStructPtr, &bufferedByteStream, ReadPNGData);

        // Setup error handling
        if (setjmp(png_jmpbuf(pngReadStructPtr)))
        {
            LOG_DEBUG("An error occurred during PNG parsing!\n");
            png_destroy_read_struct(&pngReadStructPtr, &pngInfoStructPtr, NULL);
            return;
        }

        LOG_DEBUG("Reading PNG...\n");

        // Read the entire PNG
        png_read_png(pngReadStructPtr, pngInfoStructPtr, 0, 0);

        // Retrieve the png height
        m_dimensions.height = png_get_image_height(pngReadStructPtr, pngInfoStructPtr);
        m_dimensions.width = png_get_image_width(pngReadStructPtr, pngInfoStructPtr);

        png_bytepp rows = png_get_rows(pngReadStructPtr, pngInfoStructPtr);

        for (png_uint_32 y = 0; y < m_dimensions.height; y++)
        {
            png_bytep row = rows[y];

            for (png_uint_32 x = 0; x < m_dimensions.width * 4; x++)
            {
                png_byte pixel = row[x];

                m_abgrBuffer.Write<png_byte>(pixel, (y * m_dimensions.width * 4) + x);
            }
        }

        LOG_DEBUG("PNG Height: %u\n", m_dimensions.height);

        // Delete the read struct
        png_destroy_read_struct(&pngReadStructPtr, &pngInfoStructPtr, NULL);
    }

    void Image::DrawImage(tin::ui::Canvas canvas, tin::ui::Position pos)
    {
        for (unsigned int x = 0; x < m_dimensions.width; x++)
        {
            for (unsigned int y = 0; y < m_dimensions.height; y++)
            {
                tin::ui::Colour colour;
                colour.abgr = m_abgrBuffer.Read<u32>((y * m_dimensions.width * 4) + x * 4);
                canvas.DrawPixelBlend(pos.x + x, pos.y + y, colour);
            }
        }
    }

    // Adapted from hb-menu. I mean, I did write it in the first place after all :P
    void Image::ScaleImage(tin::ui::Dimensions scaledDimensions)
    {
        tin::data::ByteBuffer newImgBuf(scaledDimensions.width * scaledDimensions.height * 4);
        const uint8_t* image = m_abgrBuffer.GetData();
        uint8_t* out = newImgBuf.GetData();

        unsigned int tmpx, tmpy;
        int pos;
        float sourceX, sourceY;
        float xScale = (float)m_dimensions.width / (float)scaledDimensions.width;
        float yScale = (float)m_dimensions.height / (float)scaledDimensions.height;
        int pixelX, pixelY;
        uint8_t r1, r2, r3, r4;
        uint8_t g1, g2, g3, g4;
        uint8_t b1, b2, b3, b4;
        uint8_t a1, a2, a3, a4;
        float fx, fy, fx1, fy1;
        int w1, w2, w3, w4;

        for (tmpx = 0; tmpx < scaledDimensions.width; tmpx++) 
        {
            for (tmpy = 0; tmpy < scaledDimensions.height; tmpy++) 
            {
                sourceX = tmpx * xScale;
                sourceY = tmpy * yScale;
                pixelX = (int)sourceX;
                pixelY = (int)sourceY;

                // get colours from four surrounding pixels
                pos = ((pixelY + 0) * m_dimensions.width + pixelX + 0) * 4;

                r1 = image[pos+0];
                g1 = image[pos+1];
                b1 = image[pos+2];
                a1 = image[pos+3];

                pos = ((pixelY + 0) * m_dimensions.width + pixelX + 1) * 4;

                r2 = image[pos+0];
                g2 = image[pos+1];
                b2 = image[pos+2];
                a2 = image[pos+3];

                pos = ((pixelY + 1) * m_dimensions.width + pixelX + 0) * 4;

                r3 = image[pos+0];
                g3 = image[pos+1];
                b3 = image[pos+2];
                a3 = image[pos+3];

                pos = ((pixelY + 1) * m_dimensions.width + pixelX + 1) * 4;

                r4 = image[pos+0];
                g4 = image[pos+1];
                b4 = image[pos+2];
                a4 = image[pos+3];

                // determine weights
                fx = sourceX - pixelX;
                fy = sourceY - pixelY;
                fx1 = 1.0f - fx;
                fy1 = 1.0f - fy;

                w1 = (int)(fx1 * fy1 * 256.0);
                w2 = (int)(fx  * fy1 * 256.0);
                w3 = (int)(fx1 * fy  * 256.0);
                w4 = (int)(fx  * fy  * 256.0);
    
                // set output pixels
                pos = ((tmpy * scaledDimensions.width) + tmpx) * 4;

                out[pos+0] = (uint8_t)((r1 * w1 + r2 * w2 + r3 * w3 + r4 * w4) >> 8);
                out[pos+1] = (uint8_t)((g1 * w1 + g2 * w2 + g3 * w3 + g4 * w4) >> 8);
                out[pos+2] = (uint8_t)((b1 * w1 + b2 * w2 + b3 * w3 + b4 * w4) >> 8);
                out[pos+3] = (uint8_t)((a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) >> 8);
            }
        }

        m_dimensions = scaledDimensions;
        m_abgrBuffer = newImgBuf;
    }
}