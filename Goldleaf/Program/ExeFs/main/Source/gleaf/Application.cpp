#include <gleaf/Goldleaf>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

extern char *fake_heap_end;

namespace gleaf
{
    static void *ghaddr;

    void Initialize()
    {
        srand(time(NULL));
        if(IsNRO() || IsQlaunch())
        {
            if(R_FAILED(svcSetHeapSize(&ghaddr, 0x10000000))) exit(1);
            if(IsNRO()) fake_heap_end = (char*)ghaddr + 0x10000000;
        }
        if(R_FAILED(ncm::Initialize())) exit(1);
        if(R_FAILED(acc::Initialize())) exit(1);
        if(R_FAILED(accountInitialize())) exit(1);
        if(R_FAILED(ncmInitialize())) exit(1);
        if(R_FAILED(ns::Initialize())) exit(1);
        if(R_FAILED(nsInitialize())) exit(1);
        if(R_FAILED(es::Initialize())) exit(1);
        if(R_FAILED(psmInitialize())) exit(1);
        if(R_FAILED(setInitialize())) exit(1);
        if(R_FAILED(setsysInitialize())) exit(1);
        if(R_FAILED(usbCommsInitialize())) exit(1);
        if(R_FAILED(lrInitialize())) exit(1);
        if(R_FAILED(splInitialize())) exit(1);
        if(R_FAILED(bpcInitialize())) exit(1);
        if(R_FAILED(gpioInitialize())) exit(1);
        horizon::InitializeGpioInputHandling();
        EnsureDirectories();
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        fs::CreateDirectory(nsys->FullPathFor("contents/temp"));
    }

    void Finalize()
    {
        fs::Explorer *nsys = fs::GetNANDSystemExplorer();
        fs::Explorer *nsfe = fs::GetNANDSafeExplorer();
        fs::Explorer *nusr = fs::GetNANDUserExplorer();
        fs::Explorer *prif = fs::GetPRODINFOFExplorer();
        fs::Explorer *sdcd = fs::GetSdCardExplorer();
        fs::DeleteDirectory(nsys->FullPathFor("contents/temp"));
        delete nsys;
        delete nsfe;
        delete nusr;
        delete prif;
        delete sdcd;
        horizon::FinalizeGpioInputHandling();
        gpioExit();
        bpcExit();
        splExit();
        lrExit();
        usbCommsExit();
        setsysExit();
        setExit();
        psmExit();
        es::Finalize();
        ns::Finalize();
        nsExit();
        accountExit();
        acc::Finalize();
        ncm::Finalize();
        ncmExit();
        if(IsNRO()) svcSetHeapSize(&ghaddr, ((u8*)envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*)ghaddr);
    }

    void EnsureDirectories()
    {
        fs::CreateDirectory("sdmc:/goldleaf");
        fs::CreateDirectory("sdmc:/goldleaf/meta");
        fs::CreateDirectory("sdmc:/goldleaf/title");
        fs::CreateDirectory("sdmc:/goldleaf/qlaunch");
        fs::CreateDirectory("sdmc:/goldleaf/dump");
        fs::CreateDirectory("sdmc:/goldleaf/userdata");
        fs::CreateDirectory("sdmc:/goldleaf/dump/temp");
        fs::CreateDirectory("sdmc:/goldleaf/dump/out");
    }

    RunMode GetRunMode()
    {
        RunMode rmode = RunMode::Unknown;
        if(envIsNso())
        {
            AppletType type = appletGetAppletType();
            switch(type)
            {
                case AppletType_Application:
                    rmode = RunMode::Title;
                    break;
                case AppletType_SystemApplet:
                    rmode = RunMode::Qlaunch;
                    break;
                default:
                    rmode = RunMode::Unknown;
                    break;
            }
        }
        else rmode = RunMode::NRO;
        return rmode;
    }

    bool IsNRO()
    {
        return (GetRunMode() == RunMode::NRO);
    }

    bool IsInstalledTitle()
    {
        return (GetRunMode() == RunMode::Title);
    }

    bool IsQlaunch()
    {
        return (GetRunMode() == RunMode::Qlaunch);
    }

    std::vector<std::string> GetSdCardCFWs()
    {
        std::vector<std::string> cfws;
        if(fs::IsDirectory("sdmc:/atmosphere")) cfws.push_back("atmosphere");
        if(fs::IsDirectory("sdmc:/reinx")) cfws.push_back("reinx");
        if(fs::IsDirectory("sdmc:/sxos")) cfws.push_back("sxos");
        return cfws;
    }

    std::vector<std::string> GetSdCardCFWNames()
    {
        std::vector<std::string> cfws;
        if(fs::IsDirectory("sdmc:/atmosphere")) cfws.push_back("Atmosphère");
        if(fs::IsDirectory("sdmc:/reinx")) cfws.push_back("ReiNX");
        if(fs::IsDirectory("sdmc:/sxos")) cfws.push_back("SX OS");
        return cfws;
    }

    std::string GetCFWName(std::string Path)
    {
        std::string name = "Unknown CFW";
        if(Path == "sdmc:/atmosphere") name = "Atmosphère";
        else if(Path == "sdmc:/reinx") name = "ReiNX";
        else if(Path == "sdmc:/sxos") name = "SX OS";
        return name;
    }

    bool HasKeyFile()
    {
        return (GetKeyFilePath() != "");
    }

    bool IsAtmosphere()
    {
        u64 tmpc = 0;
        return R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &tmpc));
    }

    bool IsReiNX()
    {
        Handle tmph = 0;
        Result rc = smRegisterService(&tmph, "rnx", false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService("rnx");
        return false;
    }

    bool IsSXOS()
    {
        Handle tmph = 0;
        Result rc = smRegisterService(&tmph, "tx", false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService("tx");
        return false;
    }

    std::string GetKeyFilePath()
    {
        std::string path = "";
        std::vector<std::string> knames = GetKeyFilePossibleNames();
        for(u32 i = 0; i < knames.size(); i++) if(fs::IsFile("sdmc:/switch/" + knames[i]))
        {
            path = "sdmc:/switch/" + knames[i];
            break;
        }
        return path;
    }

    std::vector<std::string> GetKeyFilePossibleNames()
    {
        return { "keys.dat", "prod.keys", "keys.ini", "keys.txt" };
    }
}
