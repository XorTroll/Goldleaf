
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

#include <drive/drive_Drives.hpp>

namespace drive {

    namespace {

        std::atomic_bool g_DrivesChanged = false;
        Lock g_DrivesLock;
        std::vector<UsbHsFsDevice> g_Drives;

    }

    Result Initialize() {
        GLEAF_RC_TRY(usbHsFsInitialize(0));
        
        usbHsFsSetPopulateCallback([](const UsbHsFsDevice *devices, u32 device_count, void *user_data) {
            ScopedLock lk(g_DrivesLock);
            g_Drives.clear();
            if((devices != nullptr) && (device_count > 0)) {
                g_Drives.insert(g_Drives.end(), devices, devices + device_count);
            }
            GLEAF_LOG_FMT("usbHsFsSetPopulateCallback: %u devices found", device_count);
            g_DrivesChanged = true;
        }, nullptr);

        GLEAF_RC_SUCCEED;
    }

    void Finalize() {
        usbHsFsExit();
    }

    bool GetConsumeDrivesChanged() {
        return g_DrivesChanged.exchange(false);
    }
    
    void DoWithDrives(const std::function<void(const std::vector<UsbHsFsDevice>&)> &cb) {
        ScopedLock lk(g_DrivesLock);
        cb(g_Drives);
    }

    bool UnmountDrive(const UsbHsFsDevice &drv) {
        return usbHsFsUnmountDevice(std::addressof(drv), true);
    }

}
