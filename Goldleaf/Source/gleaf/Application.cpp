#include <gleaf/Goldleaf>
#include <sys/stat.h>

extern char *fake_heap_end;

namespace gleaf
{
    static void *ghaddr;

    void Initialize()
    {
        if(!IsApplication())
        {
            if(R_FAILED(svcSetHeapSize(&ghaddr, 0x10000000))) exit(1);
            fake_heap_end = (char*)ghaddr + 0x10000000;
        }
        if(R_FAILED(ncm::Initialize())) exit(1);
        if(R_FAILED(acc::Initialize())) exit(1);
        if(R_FAILED(accountInitialize())) exit(1);
        if(R_FAILED(ncmInitialize())) exit(1);
        if(R_FAILED(ns::Initialize())) exit(1);
        if(R_FAILED(nsInitialize())) exit(1);
        if(R_FAILED(es::Initialize())) exit(1);
        if(R_FAILED(psmInitialize())) exit(1);
        if(R_FAILED(setsysInitialize())) exit(1);
        if(R_FAILED(usbCommsInitialize())) exit(1);
        if(R_FAILED(lrInitialize())) exit(1);
        EnsureDirectories();
    }

    void Finalize()
    {
        lrExit();
        usbCommsExit();
        setsysExit();
        psmExit();
        es::Finalize();
        ns::Finalize();
        nsExit();
        accountExit();
        acc::Finalize();
        ncm::Finalize();
        ncmExit();
        if(!IsApplication()) svcSetHeapSize(&ghaddr, ((u8*)envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*)ghaddr);
    }

    void EnsureDirectories()
    {
        fs::CreateDirectory("sdmc:/goldleaf");
        fs::CreateDirectory("sdmc:/goldleaf/meta");
        fs::CreateDirectory("sdmc:/goldleaf/title");
        fs::CreateDirectory("sdmc:/goldleaf/qlaunch");
        fs::CreateDirectory("sdmc:/goldleaf/dump");
        fs::CreateDirectory("sdmc:/goldleaf/dump/temp");
        fs::CreateDirectory("sdmc:/goldleaf/dump/out");
    }

    bool IsApplication()
    {
        return envIsNso();
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