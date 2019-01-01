#include <gleaf/horizon/Misc.hpp>
#include <gleaf/horizon/Title.hpp>
#include <gleaf/hactool.hpp>
#include <gleaf/fs.hpp>
#include <gleaf/Application.hpp>
#include <sstream>

namespace gleaf::horizon
{
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

    bool ExportQlaunchRomFs()
    {
        if(!HasKeyFile()) return false;
        FsFileSystem nandfs;
        fsOpenBisFileSystem(&nandfs, 31, "");
        fsdevMountDevice("qnand", nandfs);
        std::string path = "qnand:/Contents/registered/" + gleaf::horizon::GetTitleNCAPath(0x0100000000001000);
        bool ex = gleaf::hactool::Process(path, gleaf::hactool::Extraction::MakeRomFs("sdmc:/goldleaf/qlaunch"), gleaf::hactool::ExtractionFormat::NCA, GetKeyFilePath());
        if(!ex) return ex;
        path = "qnand:/Contents/registered/" + gleaf::horizon::GetTitleNCAPath(0x0100000000001013);
        ex = gleaf::hactool::Process(path, gleaf::hactool::Extraction::MakeRomFs("sdmc:/goldleaf/qlaunch"), gleaf::hactool::ExtractionFormat::NCA, GetKeyFilePath());
        fsdevUnmountDevice("qnand");
        return ex;
    }

    std::string GetCurrentTime()
    {
        time_t timet = time(NULL);
        struct tm *times = localtime((const time_t*)&timet);
        int h = times->tm_hour;
        int min = times->tm_min;
        int s = times->tm_sec;
        char timestr[9];
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
}