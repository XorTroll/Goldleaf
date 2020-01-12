
#pragma once
#include <Types.hpp>

namespace drive
{
    // Testing
    static constexpr u32 MaxDrives = 5;

    struct Drive
    {
        std::string label;
        u32 interface_id;
        u8 fs_type;
    };

    bool IsFspUsbAccessible();
    Result Initialize();
    void Exit();

    std::string FormatFileSystemType(u8 fs_type);
    std::vector<Drive> ListDrives();
    Result OpenDrive(Drive drive, FsFileSystem *out_fs);
}