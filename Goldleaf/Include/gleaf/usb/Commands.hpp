
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Types.hpp>
#include <gleaf/BufferedPlaceHolderWriter.hpp>

namespace gleaf::usb
{
    enum class CommandId
    {
        Info = 0,
        ListNSPs = 1,
        GetNSPInfo = 2,
        GetNSPContent = 3,
        GetNSPTicket = 4,
        Echo = 5
    };

    struct Command
    {
        u32 Magic;
        u32 CommandId;

        bool MagicOk();
        bool IsCommandId(usb::CommandId Id);
    } PACKED;

    struct NSPContentData
    {
        u32 Index;
        std::string Name;
        u64 Offset;
        u64 Size;
    };

    struct ContentThreadArguments
    {
        u32 Index;
        u64 Size;
        BufferedPlaceHolderWriter *WriterRef;
    };

    Command MakeCommand(CommandId Id);
    int OnContentRead(void *Args);
    int OnContentAppend(void *Args);

    static const u32 GLUC = 0x43554c47;
}