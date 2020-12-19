#include <fs/fs_DriveExplorer.hpp>

namespace fs
{
    DriveExplorer::DriveExplorer(UsbHsFsDevice &drive) : drv(drive)
    {
        auto drive_name_without_dots = String(drive.name);
        drive_name_without_dots = drive_name_without_dots.substr(0, drive_name_without_dots.length() - 1);
        this->SetNames(drive_name_without_dots, drive_name_without_dots);
    }
}