#include "data/byte_buffer.hpp"

#include "error.hpp"
#include "debug.h"

namespace tin::data
{
    ByteBuffer::ByteBuffer(size_t reserveSize)
    {
        m_buffer.resize(reserveSize);
    }

    size_t ByteBuffer::GetSize()
    {
        return m_buffer.size();
    }

    u8* ByteBuffer::GetData()
    {
        return m_buffer.data();
    }

    void ByteBuffer::Resize(size_t size)
    {
        m_buffer.resize(size, 0);
    }

    void ByteBuffer::DebugPrintContents()
    {
        fprintf(nxlinkout, "Buffer Size: 0x%lx\n", this->GetSize());
        printBytes(nxlinkout, this->GetData(), this->GetSize(), true);
    }
}