#include "util/usb_util.hpp"

#include "data/byte_buffer.hpp"
#include "debug.h"
#include "error.hpp"

namespace tin::util
{
    void USBCmdManager::SendCmd(u32 cmdId, void* data, size_t dataSize)
    {
        USBCmdHeader header;
        header.magic = 0x30435554; // TUC0 (Tinfoil USB Command 0)
        header.type = USBCmdType::REQUEST;
        header.cmdId = cmdId;
        header.dataSize = dataSize;

        USBWrite(&header, sizeof(USBCmdHeader));

        if (data && dataSize)
        {
            USBWrite(data, dataSize);
        }
    }

    void USBCmdManager::SendExitCmd()
    {
        USBCmdManager::SendCmd(0, NULL, 0);
    }

    // TODO: Add func ptr argument for response
    void USBCmdManager::SendFileRangeCmd(std::string nspName, u64 offset, u64 size)
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

        tin::data::ByteBuffer cmdData;
        cmdData.Append<FileRangeCmdHeader>(fRangeHeader);
        
        u64 nameStartOff = cmdData.GetSize();
        cmdData.Resize(cmdData.GetSize() + fRangeHeader.nspNameLen);

        strncpy((char*)cmdData.GetData() + nameStartOff, nspName.c_str(), fRangeHeader.nspNameLen);
        USBCmdManager::SendCmd(1, cmdData.GetData(), cmdData.GetSize());
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