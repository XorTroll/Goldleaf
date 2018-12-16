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
        if(R_FAILED(usbCommsInitialize())) exit(0);
        mkdir("sdmc:/switch", 777);
        mkdir("sdmc:/switch/.gleaf", 777);
        mkdir("sdmc:/switch/.gleaf/meta", 777);
        mkdir("sdmc:/switch/.gleaf/title", 777);
        mkdir("sdmc:/switch/.gleaf/out", 777);
    }

    void Finalize()
    {
        usbCommsExit();
        es::Finalize();
        ns::Finalize();
        nsExit();
        ncm::Finalize();
        ncmExit();
    }
}