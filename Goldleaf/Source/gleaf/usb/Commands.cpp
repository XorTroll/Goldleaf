#include <gleaf/usb/Commands.hpp>

namespace gleaf::usb
{
    u8 GetNumericId(CommandId Id)
    {
        return static_cast<u8>(Id);
    }

    void MakeSendCommand(u8 Id)
    {
        Command cmd;
        cmd.Magic = GLUC;
        cmd.CommandId = Id;
        cmd.Padding[0] = 0;
        cmd.Padding[1] = 0;
        cmd.Padding[2] = 0;
        WriteFixed(&cmd, sizeof(Command));
    }

    Command ReceiveCommand()
    {
        Command resp;
        ReadFixed(&resp, sizeof(Command));
        return resp;
    }
}