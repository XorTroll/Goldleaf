#include <gleaf/usb/Commands.hpp>

namespace gleaf::usb
{
    Command MakeCommand(u8 Id)
    {
        Command cmd;
        cmd.Magic = GLUC;
        cmd.CommandId = Id;
        cmd.Padding[0] = 0;
        cmd.Padding[1] = 0;
        cmd.Padding[2] = 0;
        return cmd;
    }
}