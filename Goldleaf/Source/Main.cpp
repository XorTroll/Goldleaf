#include <ui/ui_MainApplication.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

extern char *fake_heap_end;
void *new_heap_addr = NULL;

static constexpr u64 HeapSize = 0x10000000;

void Initialize()
{
    if((GetLaunchMode() == LaunchMode::Applet) && R_SUCCEEDED(svcSetHeapSize(&new_heap_addr, HeapSize))) fake_heap_end = (char*)new_heap_addr + HeapSize;
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
    if(GetLaunchMode() == LaunchMode::Applet) svcSetHeapSize(&new_heap_addr, ((u8*)envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*)new_heap_addr);
}

namespace ui
{
    extern MainApplication::Ref mainapp;
}

int main()
{
    Initialize();

    ui::mainapp = ui::MainApplication::New();
    ui::mainapp->ShowWithFadeIn();
    Finalize();
    return 0;
}