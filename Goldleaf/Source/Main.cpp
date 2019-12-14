
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_MainApplication.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

extern char *fake_heap_end;
void *new_heap_addr = NULL;

ui::MainApplication::Ref mainapp;
set::Settings gsets;
bool gupdated = false;

static constexpr u64 HeapSize = 0x10000000;

void Finalize();

void Panic(std::string msg)
{
    consoleInit(NULL);
    std::cout << std::endl;
    std::cout << "Goldleaf - panic (critical error)" << std::endl;
    std::cout << "Panic error: " << msg << std::endl;
    std::cout << std::endl;
    std::cout << "Press any key to exit Goldleaf..." << std::endl;
    consoleUpdate(NULL);

    while(true)
    {
        hidScanInput();
        if(hidKeysDown(CONTROLLER_P1_AUTO)) break;
    }

    consoleExit(NULL);
    Finalize();
    exit(0);
}

void Initialize()
{
    // if((GetLaunchMode() == LaunchMode::Applet) && R_SUCCEEDED(svcSetHeapSize(&new_heap_addr, HeapSize))) fake_heap_end = (char*)new_heap_addr + HeapSize;

    if(R_FAILED(accountInitialize(AccountServiceType_Administrator))) Panic("acc:su");
    if(R_FAILED(ncmInitialize())) Panic("ncm");
    if(R_FAILED(nsInitialize())) Panic("ns");
    if(R_FAILED(es::Initialize())) Panic("es");
    if(R_FAILED(psmInitialize())) Panic("psm");
    if(R_FAILED(setInitialize())) Panic("set");
    if(R_FAILED(setsysInitialize())) Panic("set:sys");
    if(R_FAILED(usb::detail::Initialize())) Panic("usb:ds");
    if(R_FAILED(splInitialize())) Panic("spl");
    if(R_FAILED(bpcInitialize())) Panic("bpc");
    if(R_FAILED(nifmInitialize(NifmServiceType_Admin))) Panic("nifm:a");
    if(R_FAILED(pdmqryInitialize())) Panic("pdm:qry");
    srand(time(NULL));
    EnsureDirectories();

    gsets = set::ProcessSettings();
    set::Initialize();
    if(acc::SelectFromPreselectedUser()) acc::CacheSelectedUserIcon();
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
    usb::detail::Exit();
    setsysExit();
    setExit();
    psmExit();
    es::Finalize();
    nsExit();
    accountExit();
    ncmExit();
    nifmExit();
    pdmqryExit();

    // if(GetLaunchMode() == LaunchMode::Applet) svcSetHeapSize(&new_heap_addr, ((u8*)envGetHeapOverrideAddr() + envGetHeapOverrideSize()) - (u8*)new_heap_addr);
}

int main(int argc, char **argv)
{
    Initialize();

    auto renderer = pu::ui::render::Renderer::New(SDL_INIT_EVERYTHING, pu::ui::render::RendererInitOptions::RendererEverything, pu::ui::render::RendererHardwareFlags);
    mainapp = ui::MainApplication::New(renderer);

    mainapp->Prepare();
    mainapp->Show();
    
    // If Goldleaf updated itself in this session...
    if(gupdated)
    {
        romfsExit();
        fs::DeleteFile(argv[0]);
        fs::RenameFile(consts::TempUpdatePath, argv[0]);
    }

    Finalize();
    return 0;
}