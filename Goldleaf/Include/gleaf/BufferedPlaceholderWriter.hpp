
#pragma once
#include <atomic>
#include <switch.h>
#include <memory>
#include <gleaf/ncm.hpp>

namespace gleaf
{
    static const size_t SegmentDataSize = 0x800000;

    struct BufferSegment
    {
        std::atomic_bool IsFinalized = false;
        u64 WriteOffset = 0;
        u8 Data[0x800000] = { 0 };
    };
}