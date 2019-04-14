#include <gleaf/usb/Commands.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <algorithm>

namespace gleaf::usb
{
    bool WriteCommandInput(CommandId Id)
    {
        if(!Write32(GLUC)) return false;
        return Write32(static_cast<u32>(Id));
    }
}