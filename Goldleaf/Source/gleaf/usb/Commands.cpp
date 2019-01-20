#include <gleaf/usb/Commands.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <algorithm>

namespace gleaf::usb
{
    Mutex usbmtx;

    bool Command::MagicOk()
    {
        return (this->Magic == GLUC);
    }

    bool Command::IsCommandId(usb::CommandId Id)
    {
        return (this->CommandId == static_cast<u32>(Id));
    }

    Command MakeCommand(CommandId Id)
    {
        Command cmd;
        cmd.Magic = GLUC;
        cmd.CommandId = static_cast<u32>(Id);
        return cmd;
    }

    int OnContentRead(void *Args)
    {
        return 0;
    }

    int OnContentAppend(void *Args)
    {
        return 0;
    }
}