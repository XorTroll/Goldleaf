#include <gleaf/usb/Commands.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <algorithm>

namespace gleaf::usb
{
    bool WriteCommandInput(CommandId Id)
    {
        Write32(GLUC);
        Write32(static_cast<u32>(Id));
        return true;
    }
}