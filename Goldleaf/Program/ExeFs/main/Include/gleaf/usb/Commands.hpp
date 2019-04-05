
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Types.hpp>

namespace gleaf::usb
{
    enum class CommandId
    {
        ConnectionRequest = 0,
        ConnectionResponse = 1,
        NSPName = 2,
        Start = 3,
        NSPData= 4,
        NSPContent = 5,
        NSPTicket = 6,
        Finish = 7,
    };

    enum class NewCommandId
    {
        ListSystemDrives,
        GetPathType,
        ListDirectories,
        ListFiles,
        GetFileSize,
        FileRead,
        FileWrite,
        CreateFile,
        CreateDirectory,
        DeleteFile,
        DeleteDirectory,
        RenameFile,
        RenameDirectory,
        GetDriveTotalSpace,
        GetDriveFreeSpace,
        GetNSPContents,
    };

    struct Command
    {
        u32 Magic;
        u32 CommandId;
    } PACKED;

    struct NSPContentData
    {
        u32 Index;
        std::string Name;
        u64 Offset;
        u64 Size;
    };

    Command MakeCommand(CommandId Id);
    bool CommandMagicOk(Command Cmd);
    bool IsCommandId(Command Cmd, usb::CommandId Id);

    static const u32 GLUC = 0x43554c47;

    bool WriteCommandInput(NewCommandId Id);

    static const u32 GUCI = 0x49435547;
    static const u32 GUCO = 0x4f435547;
}