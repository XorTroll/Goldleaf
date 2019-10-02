
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

bool gupdated = false;

int main(int argc, char **argv)
{
    Initialize();

    ui::mainapp = ui::MainApplication::New();
    ui::mainapp->ShowWithFadeIn();
    
    if(gupdated)
    {
        romfsExit();
        fs::DeleteFile(argv[0]);
        fs::RenameFile(TempGoldleafUpdateNro, argv[0]);
    }
    Finalize();
    return 0;
}