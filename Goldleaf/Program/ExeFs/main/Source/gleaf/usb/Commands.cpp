#include <gleaf/usb/Commands.hpp>
#include <gleaf/usb/Communications.hpp>
#include <malloc.h>
#include <algorithm>

namespace gleaf::usb
{
    Command MakeCommand(CommandId Id)
    {
        Command cmd;
        cmd.Magic = GLUC;
        cmd.CommandId = static_cast<u32>(Id);
        return cmd;
    }

    bool CommandMagicOk(Command Cmd)
    {
        return (Cmd.Magic == GLUC);
    }

    bool IsCommandId(Command Cmd, CommandId Id)
    {
        return (Cmd.CommandId == static_cast<u32>(Id));
    }
}