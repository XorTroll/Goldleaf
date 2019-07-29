#include <usb/usb_Commands.hpp>
#include <usb/usb_Communications.hpp>

namespace usb
{
    bool WriteCommandInput(CommandId Id)
    {
        if(!Write32(GLUC)) return false;
        return Write32(static_cast<u32>(Id));
    }
}