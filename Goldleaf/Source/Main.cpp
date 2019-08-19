#include <ui/ui_MainApplication.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

void Initialize()
{
    // TODO: Better way to handle this than exiting? User won't know what happened

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
    if(R_FAILED(usb::comms::Initialize())) exit(1);
    if(R_FAILED(splInitialize())) exit(1);
    if(R_FAILED(bpcInitialize())) exit(1);
    if(R_FAILED(nifmInitialize())) exit(1);
    if(R_FAILED(pdmqryInitialize())) exit(1);
    srand(time(NULL));
    EnsureDirectories();
}

void Finalize()
{
    auto fsopsbuf = fs::GetFileSystemOperationsBuffer();
    operator delete[](fsopsbuf, std::align_val_t(0x1000));
    auto nsys = fs::GetNANDSystemExplorer();
    auto nsfe = fs::GetNANDSafeExplorer();
    auto nusr = fs::GetNANDUserExplorer();
    auto prif = fs::GetPRODINFOFExplorer();
    auto sdcd = fs::GetSdCardExplorer();
    delete nsys;
    delete nsfe;
    delete nusr;
    delete prif;
    delete sdcd;
    bpcExit();
    splExit();
    usb::comms::Exit();
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
    pdmqryExit();
}

int main()
{
    Initialize();

    ui::MainApplication *mainapp = new ui::MainApplication();
    mainapp->ShowWithFadeIn();
    // Plutonium Application logic will handle closing render loop when user selects so
    Finalize();

    delete mainapp;
    return 0;
}