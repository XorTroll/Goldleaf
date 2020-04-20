#include <drive/drive_Drives.hpp>

static Service fspusb_srv;

#define DRIVE_FSPUSB_SERVICE "fsp-usb"

namespace drive
{
    bool IsFspUsbAccessible()
    {
        SmServiceName srv = smEncodeName(DRIVE_FSPUSB_SERVICE);
        Handle tmph = 0;
        Result rc = smRegisterService(&tmph, srv, false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService(srv);
        return false;
    }

    Result Initialize()
    {
        if(serviceIsActive(&fspusb_srv)) return 0;
        return smGetService(&fspusb_srv, DRIVE_FSPUSB_SERVICE);
    }

    void Exit()
    {
        if(serviceIsActive(&fspusb_srv)) serviceClose(&fspusb_srv);
    }

    std::string FormatFileSystemType(u8 fs_type)
    {
        switch(fs_type)
        {
            case 1:
                return "FAT12";
            case 2:
                return "FAT16";
            case 3:
                return "FAT32";
            case 4:
                return "exFAT";
        }
        return "...";
    }

    std::vector<Drive> ListDrives()
    {
        std::vector<Drive> drives;
        s32 drive_list[MaxDrives] = {0};
        s32 count = 0;
        auto rc = serviceDispatchOut(&fspusb_srv, 0, count,
            .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
            .buffers = { { drive_list, MaxDrives * sizeof(s32) } },
        );
        if(R_SUCCEEDED(rc))
        {
            for(u32 i = 0; i < count; i++)
            {
                auto intf_id = drive_list[i];
                u8 fs_type = 0;
                rc = serviceDispatchInOut(&fspusb_srv, 1, intf_id, fs_type);
                if(R_SUCCEEDED(rc)) {
                    char label[0x10] = {0};
                    rc = serviceDispatchIn(&fspusb_srv, 2, intf_id,
                        .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
                        .buffers = { { label, 0x10 } },
                    );
                    if(R_SUCCEEDED(rc)) {
                        Drive drv = {};
                        drv.interface_id = intf_id;
                        drv.label = label;
                        drv.fs_type = fs_type;
                        drives.push_back(drv);
                    }
                }
            }
        }
        return drives;
    }

    Result OpenDrive(Drive drive, FsFileSystem *out_fs)
    {
        return serviceDispatchIn(&fspusb_srv, 4, drive.interface_id,
            .out_num_objects = 1,
            .out_objects = &out_fs->s,
        );
    }
}