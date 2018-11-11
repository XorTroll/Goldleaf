#pragma once

#include <switch/types.h>
#include "data/byte_buffer.hpp"

namespace tin::data
{
    class ByteStream
    {
        protected:
            u64 m_offset = 0;

        public:
            virtual void ReadBytes(void* dest, size_t length) = 0;
    };

    // NOTE: This isn't generally useful, it's mainly for things like libpng
    // which rely  on streams
    class BufferedByteStream : public ByteStream
    {
        private:
            ByteBuffer m_byteBuffer;

        public:
            BufferedByteStream(ByteBuffer buffer);

            void ReadBytes(void* dest, size_t length) override;
    };
}