#include <gleaf/BufferedPlaceHolderWriter.hpp>
#include <climits>
#include <math.h>
#include <algorithm>
#include <exception>

namespace gleaf
{
    BufferedPlaceHolderWriter::BufferedPlaceHolderWriter(ncm::ContentStorage *Storage, NcmNcaId NCAId, size_t DataSize) : m_totalDataSize(DataSize), m_contentStorage(Storage), m_ncaId(NCAId)
    {
        m_bufferSegments = std::make_unique<BufferSegment[]>(BufferedPlaceHolderWriter::SegmentCount);
        if(m_bufferSegments == nullptr) { /* Error! */ }
        m_currentFreeSegmentPtr = &m_bufferSegments[m_currentFreeSegment];
        m_currentSegmentToWritePtr = &m_bufferSegments[m_currentSegmentToWrite];
    }

    void BufferedPlaceHolderWriter::AppendData(void *Buffer, size_t Length)
    {
        if((m_sizeBuffered + Length) > m_totalDataSize) { /* Error */ }
        size_t dataSizeRemaining = Length;
        u64 sourceOffset = 0;
        while(dataSizeRemaining > 0)
        {
            size_t bufferSegmentSizeRemaining = SegmentDataSize - m_currentFreeSegmentPtr->WriteOffset;
            if(m_currentFreeSegmentPtr->IsFinalized) { /* Error */ }
            if(dataSizeRemaining < bufferSegmentSizeRemaining)
            {
                memcpy(m_currentFreeSegmentPtr->Data + m_currentFreeSegmentPtr->WriteOffset, (u8*)Buffer + sourceOffset, dataSizeRemaining);
                sourceOffset += dataSizeRemaining;
                m_currentFreeSegmentPtr->WriteOffset += dataSizeRemaining;
                dataSizeRemaining = 0;
            }
            else
            {
                memcpy(m_currentFreeSegmentPtr->Data + m_currentFreeSegmentPtr->WriteOffset, (u8*)Buffer + sourceOffset, bufferSegmentSizeRemaining);
                dataSizeRemaining -= bufferSegmentSizeRemaining;
                sourceOffset += bufferSegmentSizeRemaining;
                m_currentFreeSegmentPtr->WriteOffset += bufferSegmentSizeRemaining;
                m_currentFreeSegmentPtr->IsFinalized = true;
                m_currentFreeSegment = (m_currentFreeSegment + 1) % BufferedPlaceHolderWriter::SegmentCount;
                m_currentFreeSegmentPtr = &m_bufferSegments[m_currentFreeSegment];
            }
        }
        m_sizeBuffered += Length;
        if(m_sizeBuffered == m_totalDataSize) m_currentFreeSegmentPtr->IsFinalized = true;
    }

    bool BufferedPlaceHolderWriter::CanAppendData(size_t Length)
    {
        if((m_sizeBuffered + Length) > m_totalDataSize) return false;
        if(!this->IsSizeAvailable(Length)) return false;
        return true;
    }

    void BufferedPlaceHolderWriter::WriteSegmentToPlaceHolder()
    {
        if(m_sizeWrittenToPlaceHolder >= m_totalDataSize) { /* Error */ }
        if(!m_currentSegmentToWritePtr->IsFinalized) { /* Error */ }
        size_t sizeToWriteToPlaceHolder = std::min(m_totalDataSize - m_sizeWrittenToPlaceHolder, SegmentDataSize);
        m_contentStorage->WritePlaceHolder(m_ncaId, m_sizeWrittenToPlaceHolder, m_currentSegmentToWritePtr->Data, sizeToWriteToPlaceHolder);
        m_currentSegmentToWritePtr->IsFinalized = false;
        m_currentSegmentToWritePtr->WriteOffset = 0;
        m_currentSegmentToWrite = (m_currentSegmentToWrite + 1) % BufferedPlaceHolderWriter::SegmentCount;
        m_currentSegmentToWritePtr = &m_bufferSegments[m_currentSegmentToWrite];
        m_sizeWrittenToPlaceHolder += sizeToWriteToPlaceHolder;
    }

    bool BufferedPlaceHolderWriter::CanWriteSegmentToPlaceHolder()
    {
        if(m_sizeWrittenToPlaceHolder >= m_totalDataSize) return false;
        if(!m_currentSegmentToWritePtr->IsFinalized) return false;
        return true;
    }

    u32 BufferedPlaceHolderWriter::CalculateRequiredSegmentCount(size_t Size)
    {
        if(m_currentFreeSegmentPtr->IsFinalized) return INT_MAX;
        size_t bufferSegmentSizeRemaining = SegmentDataSize - m_currentFreeSegmentPtr->WriteOffset;
        if(Size <= bufferSegmentSizeRemaining) return 1;
        else 
        {
            double numSegmentsReq = 1 + (double)(Size - bufferSegmentSizeRemaining) / (double)SegmentDataSize;
            return ceil(numSegmentsReq);
        }
    }

    bool BufferedPlaceHolderWriter::IsSizeAvailable(size_t Size)
    {
        u32 numSegmentsRequired = this->CalculateRequiredSegmentCount(Size);
        if(numSegmentsRequired > BufferedPlaceHolderWriter::SegmentCount) return false;
        for(u32 i = 0; i < numSegmentsRequired; i++)
        {
            u32 segmentIndex = m_currentFreeSegment + i;
            BufferSegment* bufferSegment = &m_bufferSegments[segmentIndex % BufferedPlaceHolderWriter::SegmentCount];
            if(bufferSegment->IsFinalized) return false;
            if(i != 0 && bufferSegment->WriteOffset != 0) { /* Error */ }
        }
        return true;
    }

    bool BufferedPlaceHolderWriter::IsBufferDataComplete()
    {
        if(m_sizeBuffered > m_totalDataSize) { /* Error */ }
        return m_sizeBuffered == m_totalDataSize;
    }

    bool BufferedPlaceHolderWriter::IsPlaceHolderComplete()
    {
        if(m_sizeWrittenToPlaceHolder > m_totalDataSize) { /* Error */ }
        return m_sizeWrittenToPlaceHolder == m_totalDataSize;
    }

    size_t BufferedPlaceHolderWriter::GetTotalDataSize()
    {
        return m_totalDataSize;
    }

    size_t BufferedPlaceHolderWriter::GetSizeBuffered()
    {
        return m_sizeBuffered;
    }

    size_t BufferedPlaceHolderWriter::GetSizeWrittenToPlaceHolder()
    {
        return m_sizeWrittenToPlaceHolder;
    }
}