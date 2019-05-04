
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Types.hpp>

namespace gleaf
{
    enum class CommandId
    {
        ListSystemDrives,
        GetEnvironmentPaths,
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
        Max,
    };

    static const u32 GLUC = 0x43554c47;
    static const u32 GLNC = 0x434e4c47;
}