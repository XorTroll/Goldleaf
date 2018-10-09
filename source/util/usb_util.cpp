#include "util/usb_util.hpp"

#include "data/byte_buffer.hpp"
#include "debug.h"
#include "error.hpp"

namespace tin::util
{
    void USBCmdManager::SendCmdHeader(u32 cmdId, size_t dataSize)
    {
        USBCmdHeader header;
        header.magic = 0x30435554; // TUC0 (Tinfoil USB Command 0)
        header.type = USBCmdType::REQUEST;
        header.cmdId = cmdId;
        header.dataSize = dataSize;

        USBWrite(&header, sizeof(USBCmdHeader));
    }

    void USBCmdManager::SendExitCmd()
    {
        USBCmdManager::SendCmdHeader(0, 0);
    }

    USBCmdHeader USBCmdManager::SendFileRangeCmd(std::string nspName, u64 offset, u64 size)
    {
        struct FileRangeCmdHeader
        {
            u64 size;
            u64 offset;
            u64 nspNameLen;
            u64 padding;
        } fRangeHeader;

        fRangeHeader.size = size;
        fRangeHeader.offset = offset;
        fRangeHeader.nspNameLen = nspName.size();
        fRangeHeader.padding = 0;

        USBCmdManager::SendCmdHeader(1, sizeof(FileRangeCmdHeader) + fRangeHeader.nspNameLen);
        USBWrite(&fRangeHeader, sizeof(FileRangeCmdHeader));
        USBWrite(nspName.c_str(), fRangeHeader.nspNameLen);

        USBCmdHeader responseHeader;
        USBRead(&responseHeader, sizeof(USBCmdHeader));
        return responseHeader;
    }

    size_t USBRead(void* out, size_t len)
    {
        u8* tmpBuf = (u8*)out;
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

    size_t USBWrite(const void* in, size_t len)
    {
    	const u8 *bufptr = (const u8 *)in;
        size_t cursize = len;
        size_t tmpsize = 0;

        while (cursize)
        {
            tmpsize = usbCommsWrite(bufptr, cursize);
            LOG_DEBUG("USB Bytes Written: \n");
            printBytes(nxlinkout, (u8*)bufptr, tmpsize, true);
            bufptr += tmpsize;
            cursize -= tmpsize;
        }

        return len;
    }
}