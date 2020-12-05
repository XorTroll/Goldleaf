#include <fs/fs_DriveExplorer.hpp>

namespace fs
{
    DriveExplorer::DriveExplorer(UsbHsFsDevice &drive) : drv(drive)
    {
        this->SetNames(drive.name, drive.name);
    }
}