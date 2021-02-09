#include <drive/drive_Drives.hpp>

namespace drive
{
    Result Initialize()
    {
        auto rc = usbHsFsInitialize(0);
        return rc;
    }

    void Exit()
    {
        usbHsFsExit();
    }

    std::vector<UsbHsFsDevice> ListDrives()
    {
        std::vector<UsbHsFsDevice> drives;
        
        auto drive_count = usbHsFsGetMountedDeviceCount();
        if(drive_count > 0)
        {
            auto drive_array = new UsbHsFsDevice[drive_count]();
            auto written = usbHsFsListMountedDevices(drive_array, drive_count);

            for(u32 i = 0; i < written; i++)
            {
                drives.push_back(drive_array[i]);
            }
            delete[] drive_array;
        }

        return drives;
    }

    bool UnmountDrive(UsbHsFsDevice &drv)
    {
        return usbHsFsUnmountDevice(&drv, false);
    }
}