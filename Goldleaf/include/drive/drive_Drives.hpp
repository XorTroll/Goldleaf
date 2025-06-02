
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <base.hpp>

namespace drive {

    struct Drive {
        std::string label;
        u32 interface_id;
        u8 fs_type;
    };

    Result Initialize();
    void Finalize();

    bool GetConsumeDrivesChanged();

    NX_CONSTEXPR const char *FormatFileSystemType(UsbHsFsDeviceFileSystemType fs_type) {
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
            case UsbHsFsDeviceFileSystemType_NTFS: {
                return "NTFS";
            }
            case UsbHsFsDeviceFileSystemType_EXT2: {
                return "EXT2";
            }
            case UsbHsFsDeviceFileSystemType_EXT3: {
                return "EXT3";
            }
            case UsbHsFsDeviceFileSystemType_EXT4: {
                return "EXT4";
            }
            default: {
                return "<unknown>";
            }
        }
    }

    NX_INLINE const char *FormatDriveFileSystemType(const UsbHsFsDevice &drive) {
        return FormatFileSystemType(static_cast<UsbHsFsDeviceFileSystemType>(drive.fs_type));
    }

    NX_CONSTEXPR bool DrivesEqual(const UsbHsFsDevice &drive_a, const UsbHsFsDevice &drive_b) {
        return (drive_a.usb_if_id == drive_b.usb_if_id) && (drive_a.lun == drive_b.lun) && (drive_a.fs_idx == drive_b.fs_idx);
    }

    NX_INLINE std::string GetDriveMountName(const UsbHsFsDevice &drive) {
        const std::string drive_name = drive.name;
        const auto drive_name_without_dots = drive_name.substr(0, drive_name.length() - 1);
        return drive_name_without_dots;
    }
    
    NX_INLINE std::string FormatDriveName(const UsbHsFsDevice &drive) {
        if(strlen(drive.product_name) > 0) {
            return drive.product_name;
        }
        else {
            return GetDriveMountName(drive);
        }
    }

    void DoWithDrives(const std::function<void(const std::vector<UsbHsFsDevice>&)> &cb);
    bool UnmountDrive(const UsbHsFsDevice &drv);

}
