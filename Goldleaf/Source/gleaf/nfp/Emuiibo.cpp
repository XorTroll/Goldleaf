#include <gleaf/nfp/Emuiibo.hpp>

Service emusrv;
u64 emucnt = 0;

namespace gleaf::nfp
{
    bool IsEmuiiboPresent()
    {
        Handle tmph = 0;
        Result rc = smRegisterService(&tmph, "nfp:emu", false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService("nfp:emu");
        return false;
    }

    Result EmuInitialize()
    {
        if(IsEmuiiboPresent()) return LibnxError_NotFound;
        atomicIncrement64(&emucnt);
        if(serviceIsActive(&emusrv)) return 0;
        return smGetService(&emusrv, "nfp:emu");
    }

    void EmuFinalize()
    {
        if(atomicDecrement64(&emucnt) == 0) serviceClose(&emusrv);
    }
}