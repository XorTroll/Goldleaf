#include <ui/ui_MainApplication.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

extern char *fake_heap_end;
static void *ghaddr;

RunMode GetRunMode()
{
    RunMode rmode = RunMode::Unknown;
    if(envIsNso())
    {
        AppletType type = appletGetAppletType();
        switch(type)
        {
            case AppletType_SystemApplication:
            case AppletType_Application:
                rmode = RunMode::Title;
                break;
            case AppletType_LibraryApplet:
                rmode = RunMode::LibraryApplet;
            default:
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

bool IsLibraryApplet()
{
    return (GetRunMode() == RunMode::LibraryApplet);
}

void Initialize()
{
    srand(time(NULL));
    if(IsNRO())
    {
        if(R_SUCCEEDED(svcSetHeapSize(&ghaddr, 0x10000000))) fake_heap_end = (char*)ghaddr + 0x10000000;
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
    if(R_FAILED(splInitialize())) exit(1);
    if(R_FAILED(bpcInitialize())) exit(1);
    if(R_FAILED(nifmInitialize())) exit(1);
    EnsureDirectories();
}

void Finalize()
{
    fs::NANDExplorer *nsys = fs::GetNANDSystemExplorer();
    fs::NANDExplorer *nsfe = fs::GetNANDSafeExplorer();
    fs::NANDExplorer *nusr = fs::GetNANDUserExplorer();
    fs::NANDExplorer *prif = fs::GetPRODINFOFExplorer();
    fs::SdCardExplorer *sdcd = fs::GetSdCardExplorer();
    delete nsys;
    delete nsfe;
    delete nusr;
    delete prif;
    delete sdcd;
    bpcExit();
    splExit();
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
    nifmExit();
    if(IsNRO()) svcSetHeapSize(&ghaddr, ((u8*)envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*)ghaddr);
}

int main()
{
    Initialize();
    ui::MainApplication *mainapp = new ui::MainApplication();
    mainapp->ShowWithFadeIn();
    Finalize();
    delete mainapp;
    return 0;
}