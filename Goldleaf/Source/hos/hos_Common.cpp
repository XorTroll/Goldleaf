#include <hos/hos_Common.hpp>
#include <hos/hos_Titles.hpp>
#include <fs/fs_Explorer.hpp>
#include <sstream>

namespace hos
{
    alignas(0x1000) u8 workpage[0x1000];
    alignas(0x1000) u8 clearblock[0x1000];

    u32 GetBatteryLevel()
    {
        u32 bat = 0;
        psmGetBatteryChargePercentage(&bat);
        return bat;
    }

    bool IsCharging()
    {
        ChargerType charger = ChargerType_None;
        psmGetChargerType(&charger);
        return (charger > ChargerType_None);
    }

    std::string GetCurrentTime()
    {
        time_t timet = time(NULL);
        struct tm *times = localtime((const time_t*)&timet);
        int h = times->tm_hour;
        int min = times->tm_min;
        int s = times->tm_sec;
        char timestr[0x10];
        sprintf(timestr, "%02d:%02d:%02d", h, min, s);
        return std::string(timestr);
    }

    FwVersion GetFwVersion()
    {
        FwVersion pfw = { 0, 0, 0, "" };
        SetSysFirmwareVersion fw;
        Result rc = setsysGetFirmwareVersion(&fw);
        if(rc != 0) return pfw;
        pfw.Major = fw.major;
        pfw.Minor = fw.minor;
        pfw.Micro = fw.micro;
        pfw.DisplayName = std::string(fw.display_title);
        return pfw;
    }

    std::string FwVersion::ToString()
    {
        return (std::to_string(this->Major) + "." + std::to_string(this->Minor) + "." + std::to_string(this->Micro));
    }

    std::string FormatHex(u32 Number)
    {
        std::stringstream strm;
        strm << "0x" << std::hex << Number;
        return strm.str();
    }

    std::string FormatHex128(u128 Number)
    {
        u8 *ptr = (u8*)&Number;
        std::string res;
        for(u32 i = 0; i < 16; i++)
        {
            std::stringstream strm;
            strm << std::hex << (int)ptr[i];
            res += strm.str();
        }
        return res;
    }

    std::string DoubleToString(double Number)
    {
        std::stringstream strm;
        strm << Number;
        return strm.str();
    }

    u64 GetSdCardFreeSpaceForInstalls()
    {
        return fs::GetFreeSpaceForPartition(fs::Partition::SdCard);
    }

    u64 GetNANDFreeSpaceForInstalls()
    {
        return fs::GetFreeSpaceForPartition(fs::Partition::NANDUser);
    }

    void IRAMWrite(void *Data, uintptr_t IRAMAddress, size_t Size)
    {
        memcpy(workpage, Data, Size);
        SecmonArgs args = { 0 };
        args.X[0] = 0xf0000201;
        args.X[1] = (uintptr_t)workpage;
        args.X[2] = IRAMAddress;
        args.X[3] = Size;
        args.X[4] = 1;
        svcCallSecureMonitor(&args);
        memcpy(Data, workpage, Size);
    }

    void IRAMClear()
    {
        memset(clearblock, 0xff, 0x1000);
        for(u32 i = 0; i < 0x2f000; i += 0x1000) IRAMWrite(clearblock, (0x40010000 + i), 0x1000);
    }

    void PayloadProcess(std::string Path)
    {
        alignas(0x1000) u8 payload[0x2f000];
        auto fexp = fs::GetExplorerForMountName(fs::GetPathRoot(Path));
        std::vector<u8> data = fexp->ReadFile(Path);
        if(!data.empty())
        {
            memcpy(payload, data.data(), std::min(0x2f000, (int)data.size()));
            IRAMClear();
            for(u32 i = 0; i < 0x2f000; i += 0x1000) IRAMWrite(&payload[i], (0x40010000 + i), 0x1000);
            splSetConfig((SplConfigItem)65001, 2);
        }
    }
}