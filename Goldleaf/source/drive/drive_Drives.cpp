
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

#include <drive/drive_Drives.hpp>

namespace drive {

    Result Initialize() {
        return usbHsFsInitialize(0);
    }

    void Exit() {
        usbHsFsExit();
    }

    std::vector<UsbHsFsDevice> ListDrives() {
        std::vector<UsbHsFsDevice> drives;
        
        const auto drive_count = usbHsFsGetMountedDeviceCount();
        if(drive_count > 0) {
            auto drive_array = new UsbHsFsDevice[drive_count]();
            const auto written = usbHsFsListMountedDevices(drive_array, drive_count);

            for(u32 i = 0; i < written; i++) {
                drives.push_back(drive_array[i]);
            }
            delete[] drive_array;
        }

        return drives;
    }

    bool UnmountDrive(UsbHsFsDevice &drv) {
        return usbHsFsUnmountDevice(&drv, false);
    }

}