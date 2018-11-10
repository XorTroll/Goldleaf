#include <gleaf/ByteBuffer.hpp>

namespace gleaf
{
    ByteBuffer::ByteBuffer(size_t ReserveSize)
    {
        buffer.resize(ReserveSize);
    }

    size_t ByteBuffer::GetSize()
    {
        return buffer.size();
    }

    u8 *ByteBuffer::GetData()
    {
        return buffer.data();
    }

    void ByteBuffer::Resize(size_t Size)
    {
        buffer.resize(Size, 0);
    }
}