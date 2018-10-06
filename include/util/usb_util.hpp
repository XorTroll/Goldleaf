#pragma once

#include "nx/ipc/usb_comms_new.h"

namespace tin::util
{
    size_t USBRead(void* buf, size_t len);
    size_t USBWrite(const void* out, size_t len);
}