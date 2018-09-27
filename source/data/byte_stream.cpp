#include "data/byte_stream.hpp"

namespace tin::data
{
    BufferedByteStream::BufferedByteStream(ByteBuffer buffer) :
        m_byteBuffer(buffer)
    {

    }

    void BufferedByteStream::ReadBytes(void* dest, size_t length)
    {
        if (m_offset + length > m_byteBuffer.GetSize())
            return;

        memcpy(dest, m_byteBuffer.GetData() + m_offset, length);
        m_offset += length;
    }
}