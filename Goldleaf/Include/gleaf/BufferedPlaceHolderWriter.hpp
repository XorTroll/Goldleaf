
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <atomic>
#include <switch.h>
#include <memory>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    static const size_t SegmentDataSize = 1048576;

    struct BufferSegment
    {
        std::atomic_bool IsFinalized = false;
        u64 WriteOffset = 0;
        u8 Data[1048576] = { 0 };
    };

    class BufferedPlaceHolderWriter
    {
        public:
            static const int SegmentCount = 4;
            BufferedPlaceHolderWriter(ncm::ContentStorage *Storage, NcmNcaId NCAId, size_t DataSize);
            void AppendData(void *Buffer, size_t Length);
            bool CanAppendData(size_t Length);
            void WriteSegmentToPlaceHolder();
            bool CanWriteSegmentToPlaceHolder();
            u32 CalculateRequiredSegmentCount(size_t Size);
            bool IsSizeAvailable(size_t Size);
            bool IsBufferDataComplete();
            bool IsPlaceHolderComplete();
            size_t GetTotalDataSize();
            size_t GetSizeBuffered();
            size_t GetSizeWrittenToPlaceHolder();
        private:
            size_t m_totalDataSize = 0;
            size_t m_sizeBuffered = 0;
            size_t m_sizeWrittenToPlaceHolder = 0;
            u64 m_currentFreeSegment = 0;
            BufferSegment* m_currentFreeSegmentPtr = NULL;
            u64 m_currentSegmentToWrite = 0;
            BufferSegment* m_currentSegmentToWritePtr = NULL;
            std::unique_ptr<BufferSegment[]> m_bufferSegments;
            ncm::ContentStorage* m_contentStorage;
            NcmNcaId m_ncaId;
    };
}