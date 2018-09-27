#include "ui/framework/image.hpp"

#include "data/byte_stream.hpp"
#include "error.hpp"

namespace tin::ui
{
    Image::Image()
    {
        
    }

    Image::Image(tin::data::ByteBuffer& buffer, ImageType type)
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
        m_height = png_get_image_height(pngReadStructPtr, pngInfoStructPtr);
        m_width = png_get_image_width(pngReadStructPtr, pngInfoStructPtr);

        png_bytepp rows = png_get_rows(pngReadStructPtr, pngInfoStructPtr);

        for (png_uint_32 y = 0; y < m_height; y++)
        {
            png_bytep row = rows[y];

            for (png_uint_32 x = 0; x < m_width * 4; x++)
            {
                png_byte pixel = row[x];

                m_rgbaBuffer.Write<png_byte>(pixel, (y * m_width * 4) + x);
            }
        }

        LOG_DEBUG("PNG Height: %u\n", m_height);

        // Delete the read struct
        png_destroy_read_struct(&pngReadStructPtr, &pngInfoStructPtr, NULL);
    }

    void Image::DrawImage(Canvas canvas, Position pos)
    {
        for (unsigned int x = 0; x < m_width; x++)
        {
            for (unsigned int y = 0; y < m_height; y++)
            {
                tin::ui::Colour colour;
                colour.abgr = m_rgbaBuffer.Read<u32>((y * m_width * 4) + x * 4);
                canvas.DrawPixelBlend(pos.x + x, pos.y + y, colour);
            }
        }
    }
}