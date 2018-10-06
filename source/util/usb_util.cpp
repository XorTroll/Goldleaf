#include "util/usb_util.hpp"

namespace tin::util
{
    size_t USBRead(void* buf, size_t len)
    {
        u8* tmpBuf = (u8*)buf;
        size_t sizeRemaining = len;
        size_t tmpSizeRead = 0;

        while (sizeRemaining)
        {
            tmpSizeRead = usbCommsRead(tmpBuf, sizeRemaining);
            tmpBuf += tmpSizeRead;
            sizeRemaining -= tmpSizeRead;
        }

        return len;
    }

    size_t USBWrite(const void* out, size_t len)
    {
        const u8* tmpBuf = (const u8*)out;
        size_t sizeRemaining = len;
        size_t tmpSizeRead = 0;

        while (sizeRemaining)
        {
            tmpSizeRead = usbCommsWrite(tmpBuf, sizeRemaining);
            tmpBuf += tmpSizeRead;
            sizeRemaining -= tmpSizeRead;
        }

        return len;
    }
}