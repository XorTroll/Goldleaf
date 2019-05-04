
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Credits to Stary for the base of USB-drive support!

*/

#pragma once
#include <switch.h>
#include <gleaf/drive/SCSIContext.hpp>

namespace gleaf::drive
{
    #include <fatfs/fatfs.hpp>

    struct Drive
    {
        UsbHsClientIfSession client;
        UsbHsClientEpSession inep;
        UsbHsClientEpSession outep;
        SCSIDevice *base;
        SCSIBlock *block;
        FATFS *fs;
        char mountname[10];
        bool mounted;
    };

    enum class DriveStatus
    {
        Ok,
        Bad,
    };

    enum class FileSystemType
    {
        Invalid,
        FAT12,
        FAT16,
        FAT32,
        exFAT,
    };

    Result Initialize();
    bool IsInitialized();
    Result WaitForDrives(s64 timeout = -1);
    Result UpdateAvailableInterfaces(s64 timeout);
    void Exit();
    Result CountDrives(s32 *out);
    Result OpenDrive(s32 idx, Drive *out);
    Result DriveMount(Drive *drv, const char *name);
    Result DriveUnmount(Drive *drv);
    bool DriveIsMounted(Drive *drv);
    FATFS *DriveGetFileSystem(Drive *drv);
    const char *DriveGetVolumeLabel(Drive *drv);
    FileSystemType DriveGetType(Drive *drv);
    Result DriveIsOk(Drive *drv, DriveStatus *out);
    void DriveClose(Drive *drv);
    bool DriveIsActive(Drive *drv);
    bool DriveIsClosed(Drive *drv);
}