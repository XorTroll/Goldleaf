
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/usb/Communications.hpp>

namespace gleaf::usb
{
    enum class CommandType
    {
        Request,
        Response,
    };

    enum class CommandId
    {
        ConnectionRequest = 0,
        ConnectionResponse = 1,
        Content = 2,
        Finish = 3,
    };

    struct Command
    {
        u32 Magic;
        u8 CommandId;
        u8 Padding[3];
    } PACKED;

    u8 GetNumericId(CommandId Id);
    void MakeSendCommand(u8 Id);
    Command ReceiveCommand();

    static const u32 GLUC = 0x43554c47;
}