#include <gleaf/Goldleaf>
#include <sys/stat.h>

namespace gleaf
{
    void Initialize()
    {
        if(R_FAILED(ncm::Initialize())) exit(0);
        if(R_FAILED(ncmInitialize())) exit(0);
        if(R_FAILED(ns::Initialize())) exit(0);
        if(R_FAILED(nsInitialize())) exit(0);
        if(R_FAILED(es::Initialize())) exit(0);
        if(R_FAILED(psmInitialize())) exit(0);
        if(R_FAILED(setsysInitialize())) exit(0);
        if(R_FAILED(usbCommsInitialize())) exit(0);
        if(R_FAILED(lrInitialize())) exit(0);
        fs::CreateDirectory("sdmc:/goldleaf");
        fs::CreateDirectory("sdmc:/goldleaf/meta");
        fs::CreateDirectory("sdmc:/goldleaf/title");
        fs::CreateDirectory("sdmc:/goldleaf/qlaunch");
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
        ncm::Finalize();
        ncmExit();
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
        else if(Path == "sdmc:/sxos") name == "SX OS";
        return name;
    }
}