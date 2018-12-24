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
        gleaf::fs::CreateDirectory("sdmc:/switch");
        gleaf::fs::CreateDirectory("sdmc:/switch/.gleaf");
        gleaf::fs::CreateDirectory("sdmc:/switch/.gleaf/meta");
        gleaf::fs::CreateDirectory("sdmc:/switch/.gleaf/title");
        gleaf::fs::CreateDirectory("sdmc:/switch/.gleaf/qlaunch");
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
}