
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

    inline std::string FormatFileSystemType(UsbHsFsDeviceFileSystemType fs_type)
    {
        switch(fs_type)
        {
            case UsbHsFsDeviceFileSystemType_FAT12:
                return "FAT12";
            case UsbHsFsDeviceFileSystemType_FAT16:
                return "FAT16";
            case UsbHsFsDeviceFileSystemType_FAT32:
                return "FAT32"; 
            case UsbHsFsDeviceFileSystemType_exFAT:
                return "exFAT";
        }
        return "<unknown>";
    }

    constexpr inline bool DrivesEqual(UsbHsFsDevice &a, UsbHsFsDevice &b)
    {
        return (a.usb_if_id == b.usb_if_id) && (a.lun == b.lun) && (a.fs_idx == b.fs_idx);
    }

    std::vector<UsbHsFsDevice> ListDrives();
}