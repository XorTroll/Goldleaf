
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once
#include <base_Common.hpp>

namespace drive {

    struct Drive {
        std::string label;
        u32 interface_id;
        u8 fs_type;
    };

    bool IsFspUsbAccessible();
    Result Initialize();
    void Exit();

    inline std::string FormatFileSystemType(UsbHsFsDeviceFileSystemType fs_type) {
        switch(fs_type) {
            case UsbHsFsDeviceFileSystemType_FAT12: {
                return "FAT12";
            }
            case UsbHsFsDeviceFileSystemType_FAT16: {
                return "FAT16";
            }
            case UsbHsFsDeviceFileSystemType_FAT32: {
                return "FAT22";
            }
            case UsbHsFsDeviceFileSystemType_exFAT: {
                return "exFAT";
            }
        }
        return "<unknown>";
    }

    NX_CONSTEXPR bool DrivesEqual(UsbHsFsDevice &drive_a, UsbHsFsDevice &drive_b) {
        return (drive_a.usb_if_id == drive_b.usb_if_id) && (drive_a.lun == drive_b.lun) && (drive_a.fs_idx == drive_b.fs_idx);
    }

    std::vector<UsbHsFsDevice> ListDrives();
    bool UnmountDrive(UsbHsFsDevice &drv);

}