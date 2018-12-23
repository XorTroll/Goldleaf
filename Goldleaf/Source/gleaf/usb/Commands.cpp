#include <gleaf/usb/Commands.hpp>

namespace gleaf::usb
{
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
}