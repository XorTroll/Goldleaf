#include <gleaf/gleaf.hpp>

namespace gleaf
{
    void Initialize()
    {
        if(R_FAILED(ncm::Initialize())) ThrowError("Failed to initialize service: NCM / 'ncm'");
        if(R_FAILED(ncmInitialize())) ThrowError("Failed to initialize service: NCM / 'ncm'");
        if(R_FAILED(ns::Initialize())) ThrowError("Failed to initialize service: NS / 'ns:am' / 'ns:am2'");
        if(R_FAILED(nsInitialize())) ThrowError("Failed to initialize service: NS / 'ns:am' / 'ns:am2'");
        if(R_FAILED(es::Initialize())) ThrowError("Failed to initialize service: ES / 'es'");
        // if(R_FAILED(nifmInitialize())) ThrowError("Failed to initialize service: NIFM / 'nifm:u'");
        // if(R_FAILED(setInitialize())) ThrowError("Failed to initialize service: SET / 'set'");
        // if(R_FAILED(plInitialize())) ThrowError("Failed to initialize service: PL / 'pl:u'");
        // if(R_FAILED(romfsInit())) ThrowError("Failed to initialize RomFS");
        // if(R_FAILED(usbCommsInitialize())) ThrowError("Failed to initialize service: USB / 'usb:ds'");
    }

    void Finalize()
    {
        // nifmExit();
        // usbCommsExit();
        // romfsExit();
        // plExit();
        // setExit();
        ncm::Finalize();
        ncmExit();
        ns::Finalize();
        nsExit();
        es::Finalize();
    }
}