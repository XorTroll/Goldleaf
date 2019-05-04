#include <gleaf/drive/Drive.hpp>
#include <cstring>
#include <string>

extern gleaf::drive::SCSIBlockPartition *drive_block;

namespace gleaf::drive
{
    static bool init = false;
    static UsbHsInterfaceFilter ifilter;
    static Event ifaceavailable;
    static UsbHsInterface ifaces[8];
    static s32 icount = 0;

    Result Initialize()
    {
        if(init) return 0;
        Result rc = usbHsInitialize();
        if(rc == 0)
        {
            memset(&ifilter, 0, sizeof(ifilter));
            ifilter.Flags = UsbHsInterfaceFilterFlags_bInterfaceClass | UsbHsInterfaceFilterFlags_bInterfaceSubClass | UsbHsInterfaceFilterFlags_bInterfaceProtocol;
            ifilter.bInterfaceClass = 8;
            ifilter.bInterfaceSubClass = 6;
            ifilter.bInterfaceProtocol = 80;
            rc = usbHsCreateInterfaceAvailableEvent(&ifaceavailable, false, 0, &ifilter);
            if(rc == 0) init = true;
        }
        return rc;
    }

    bool IsInitialized()
    {
        return init;
    }

    Result WaitForDrives(s64 timeout)
    {
        if(!init) return LibnxError_NotInitialized;
        return UpdateAvailableInterfaces(timeout);
    }

    Result UpdateAvailableInterfaces(s64 timeout)
    {
        Result rc = eventWait(&ifaceavailable, timeout);
        if(rc != 0) return rc;
        memset(ifaces, 0, sizeof(ifaces));
        rc = usbHsQueryAvailableInterfaces(&ifilter, ifaces, sizeof(ifaces), &icount);
        return rc;
    }

    void Exit()
    {
        if(!init) return;
        usbHsDestroyInterfaceAvailableEvent(&ifaceavailable, 0);
        usbHsExit();
        init = false;
    }

    Result CountDrives(s32 *out)
    {
        UpdateAvailableInterfaces(-1);
        *out = icount;
        return 0;
    }

    Result OpenDrive(s32 idx, Drive *out)
    {
        if(DriveIsActive(out)) return 0;
        UpdateAvailableInterfaces(-1);
        if((idx + 1) > icount) return LibnxError_BadInput;
        memset(&out->client, 0, sizeof(UsbHsClientIfSession));
        memset(&out->inep, 0, sizeof(UsbHsClientEpSession));
        memset(&out->outep, 0, sizeof(UsbHsClientEpSession));
        Result rc = usbHsAcquireUsbIf(&out->client, &ifaces[idx]);
        if(rc == 0)
        {
            for(u32 i = 0; i < 15; i++)
            {
                auto epd = &out->client.inf.inf.input_endpoint_descs[i];
                if(epd->bLength > 0)
                {
                    rc = usbHsIfOpenUsbEp(&out->client, &out->inep, 1, epd->wMaxPacketSize, epd);
                    break;
                }
            }
            for(u32 i = 0; i < 15; i++)
            {
                auto epd = &out->client.inf.inf.output_endpoint_descs[i];
                if(epd->bLength > 0)
                {
                    rc = usbHsIfOpenUsbEp(&out->client, &out->outep, 1, epd->wMaxPacketSize, epd);
                    break;
                }
            }
            if(rc == 0)
            {
                out->base = new SCSIDevice(&out->client, &out->inep, &out->outep);
            }
        }
        return rc;
    }

    Result DriveMount(Drive *drv, const char *name)
    {
        if(drv->mounted) return 0;
        drv->block = new SCSIBlock(drv->base);
        drive_block = &drv->block->partitions[0];
        drv->fs = (FATFS*)malloc(sizeof(FATFS));
        FRESULT rc = f_mount(drv->fs, name, 1);
        if(rc != FR_OK) return MAKERESULT(199, rc);
        memset(drv->mountname, 0, 10);
        strcpy(drv->mountname, name);
        drv->mounted = true;
        return 0;
    }

    Result DriveUnmount(Drive *drv)
    {
        if(!drv->mounted) return 0;
        f_mount(NULL, drv->mountname, 1);
        free(drv->fs);
        delete drv->block;
        drv->mounted = false;
        return 0;
    }

    bool DriveIsMounted(Drive *drv)
    {
        return drv->mounted;
    }

    FATFS *DriveGetFileSystem(Drive *drv)
    {
        return drv->fs;
    }

    const char *DriveGetVolumeLabel(Drive *drv)
    {
        if(!DriveIsMounted(drv)) return NULL;
        char out[256];
        f_getlabel((std::string(drv->mountname) + ":").c_str(), out, NULL);
        return out;
    }

    FileSystemType DriveGetType(Drive *drv)
    {
        u8 rawtype = drv->fs->fs_type;
        FileSystemType fst = FileSystemType::Invalid;
        if(rawtype == FS_FAT12) fst = FileSystemType::FAT12;
        else if(rawtype == FS_FAT16) fst = FileSystemType::FAT16;
        else if(rawtype == FS_FAT32) fst = FileSystemType::FAT32;
        else if(rawtype == FS_EXFAT) fst = FileSystemType::exFAT;
        return fst;
    }

    Result DriveIsOk(Drive *drv, DriveStatus *out)
    {
        DriveStatus dvst = DriveStatus::Bad;
        UsbHsInterface aqifaces[8];
        memset(aqifaces, 0, sizeof(aqifaces));
        s32 aqicount = 0;
        Result rc = usbHsQueryAcquiredInterfaces(aqifaces, sizeof(aqifaces), &aqicount);
        if(rc == 0)
        {
            for(s32 i = 0; i < aqicount; i++)
            {
                UsbHsInterface *iface = &aqifaces[i];
                if(iface->inf.ID == drv->client.inf.inf.ID)
                {
                    dvst = DriveStatus::Ok;
                    break;
                }
            }
        }
        *out = dvst;
        return rc;
    }

    void DriveClose(Drive *drv)
    {
        if(DriveIsClosed(drv)) return;
        if(DriveIsMounted(drv)) DriveUnmount(drv);
        usbHsEpClose(&drv->inep);
        usbHsEpClose(&drv->outep);
        usbHsIfResetDevice(&drv->client);
        usbHsIfClose(&drv->client);
        delete drv->base;
    }

    bool DriveIsActive(Drive *drv)
    {
        return serviceIsActive(&drv->client.s);
    }

    bool DriveIsClosed(Drive *drv)
    {
        return !DriveIsActive(drv);
    }
}