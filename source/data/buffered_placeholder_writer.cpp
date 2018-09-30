#include "data/buffered_placeholder_writer.hpp"

#include <math.h>
#include <algorithm>
#include <exception>
#include "error.hpp"
#include "debug.h"

namespace tin::data
{
    BufferedPlaceholderWriter::BufferedPlaceholderWriter(nx::ncm::ContentStorage* contentStorage, NcmNcaId ncaId, size_t totalDataSize) :
        m_totalDataSize(totalDataSize), m_contentStorage(contentStorage), m_ncaId(ncaId)
    {
        // Though currently the number of segments is fixed, we want them allocated on the heap, not the stack
        m_bufferSegments = std::make_unique<BufferSegment[]>(NUM_BUFFER_SEGMENTS);
    }

    void BufferedPlaceholderWriter::AppendData(void* source, size_t length)
    {
        if (m_sizeBuffered + length > m_totalDataSize)
            THROW_FORMAT("Cannot append data as it would exceed the expected total.\n");

        size_t dataSizeRemaining = length;
        u64 sourceOffset = 0;

        while (dataSizeRemaining > 0)
        {
            BufferSegment* currentBufferSegment = &m_bufferSegments[m_currentFreeSegment];
            size_t bufferSegmentSizeRemaining = BUFFER_SEGMENT_DATA_SIZE - currentBufferSegment->writeOffset;

            if (currentBufferSegment->isFinalized)
                THROW_FORMAT("Current buffer segment is already finalized!\n");

            if (dataSizeRemaining < bufferSegmentSizeRemaining)
            {
                memcpy(currentBufferSegment->data + currentBufferSegment->writeOffset, (u8*)source + sourceOffset, dataSizeRemaining);
                sourceOffset += dataSizeRemaining;
                currentBufferSegment->writeOffset += dataSizeRemaining;
                dataSizeRemaining = 0;
            }
            else
            {
                memcpy(currentBufferSegment->data + currentBufferSegment->writeOffset, (u8*)source + sourceOffset, bufferSegmentSizeRemaining);
                dataSizeRemaining -= bufferSegmentSizeRemaining;
                sourceOffset += bufferSegmentSizeRemaining;
                currentBufferSegment->writeOffset += bufferSegmentSizeRemaining;
                currentBufferSegment->isFinalized = true;
                
                m_currentFreeSegment = (m_currentFreeSegment + 1) % NUM_BUFFER_SEGMENTS;
            }
        }

        m_sizeBuffered += length;

        if (m_sizeBuffered == m_totalDataSize)
        {
            BufferSegment* currentBufferSegment = &m_bufferSegments[m_currentFreeSegment];
            currentBufferSegment->isFinalized = true;
        }
    }

    bool BufferedPlaceholderWriter::CanAppendData(size_t length)
    {
        if (m_sizeBuffered + length > m_totalDataSize)
            return false;

        if (!this->IsSizeAvailable(length))
            return false;

        return true;
    }

    void BufferedPlaceholderWriter::WriteSegmentToPlaceholder()
    {
        if (m_sizeWrittenToPlaceholder >= m_totalDataSize)
            THROW_FORMAT("Cannot write segment as end of data has already been reached!\n");

        BufferSegment* currentBufferSegment = &m_bufferSegments[m_currentSegmentToWrite];

        if (!currentBufferSegment->isFinalized)
            THROW_FORMAT("Cannot write segment as it hasn't been finalized!\n");

        // NOTE: The final segment will have leftover data from previous writes, however
        // this will be accounted for by this size
        size_t sizeToWriteToPlaceholder = std::min(m_totalDataSize - m_sizeWrittenToPlaceholder, BUFFER_SEGMENT_DATA_SIZE);
        m_contentStorage->WritePlaceholder(m_ncaId, m_sizeWrittenToPlaceholder, currentBufferSegment->data, sizeToWriteToPlaceholder);

        currentBufferSegment->isFinalized = false;
        currentBufferSegment->writeOffset = 0;
        m_currentSegmentToWrite = (m_currentSegmentToWrite + 1) % NUM_BUFFER_SEGMENTS;
        m_sizeWrittenToPlaceholder += sizeToWriteToPlaceholder;
    }

    bool BufferedPlaceholderWriter::CanWriteSegmentToPlaceholder()
    {
        if (m_sizeWrittenToPlaceholder >= m_totalDataSize)
            return false;

        BufferSegment* currentBufferSegment = &m_bufferSegments[m_currentSegmentToWrite];

        if (!currentBufferSegment->isFinalized)
            return false;

        return true;
    }

    u32 BufferedPlaceholderWriter::CalcNumSegmentsRequired(size_t size)
    {
        BufferSegment* currentBufferSegment = &m_bufferSegments[m_currentFreeSegment];

        //LOG_DEBUG("Size: %lu\n", size);

        if (currentBufferSegment->isFinalized)
            THROW_FORMAT("Current buffer segment is already finalized!\n");

        size_t bufferSegmentSizeRemaining = BUFFER_SEGMENT_DATA_SIZE - currentBufferSegment->writeOffset;

        //LOG_DEBUG("Buffer segment size remaining: %lu\n", bufferSegmentSizeRemaining);

        if (size <= bufferSegmentSizeRemaining) return 1;
        else 
        {
            double numSegmentsReq = 1 + (double)(size - bufferSegmentSizeRemaining) / (double)BUFFER_SEGMENT_DATA_SIZE;
            return ceil(numSegmentsReq);
        }
    }

    bool BufferedPlaceholderWriter::IsSizeAvailable(size_t size)
    {
        u32 numSegmentsRequired = this->CalcNumSegmentsRequired(size);

        if (numSegmentsRequired > NUM_BUFFER_SEGMENTS)
            return false;

        for (unsigned int i = 0; i < numSegmentsRequired; i++)
        {
            unsigned int segmentIndex = m_currentFreeSegment + i;
            BufferSegment* bufferSegment = &m_bufferSegments[segmentIndex % NUM_BUFFER_SEGMENTS];
        
            if (bufferSegment->isFinalized)
                return false;

            if (i != 0 && bufferSegment->writeOffset != 0)
                THROW_FORMAT("Unexpected non-zero write offset at segment %u (%lu)\n", segmentIndex, bufferSegment->writeOffset);
        }

        return true;
    }

    bool BufferedPlaceholderWriter::IsBufferDataComplete()
    {
        if (m_sizeBuffered > m_totalDataSize)
            THROW_FORMAT("Size buffered cannot exceed total data size!\n");

        return m_sizeBuffered == m_totalDataSize;
    }

    bool BufferedPlaceholderWriter::IsPlaceholderComplete()
    {
        if (m_sizeWrittenToPlaceholder > m_totalDataSize)
            THROW_FORMAT("Size written to placeholder cannot exceed total data size!\n");

        return m_sizeWrittenToPlaceholder == m_totalDataSize;
    }

    size_t BufferedPlaceholderWriter::GetTotalDataSize()
    {
        return m_totalDataSize;
    }

    size_t BufferedPlaceholderWriter::GetSizeBuffered()
    {
        return m_sizeBuffered;
    }

    size_t BufferedPlaceholderWriter::GetSizeWrittenToPlaceholder()
    {
        return m_sizeWrittenToPlaceholder;
    }

    void BufferedPlaceholderWriter::DebugPrintBuffers()
    {
        LOG_DEBUG("BufferedPlaceholderWriter Buffers: \n");

        for (int i = 0; i < NUM_BUFFER_SEGMENTS; i++)
        {
            LOG_DEBUG("Buffer %u:\n", i);
            printBytes(nxlinkout, m_bufferSegments[i].data, BUFFER_SEGMENT_DATA_SIZE, true);
        }
    }
}