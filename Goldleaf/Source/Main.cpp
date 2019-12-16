
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

extern char *fake_heap_end;
void *new_heap_addr = NULL;
extern char** __system_argv;

ui::MainApplication::Ref mainapp;
set::Settings gsets;
bool gupdated = false;

alignas(16) u8 __nx_exception_stack[0x1000];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

u64 GetBaseAddress()
{
    u32 p;
    MemoryInfo info;

    // Find the memory region in which the function itself is stored.
    // The start of it will be the base address the homebrew was mapped to.
    svcQueryMemory(&info, &p, (u64) &GetBaseAddress);

    return info.addr;
}

struct StackFrame
{
    u64 fp;     // Frame Pointer (Pointer to previous stack frame)
    u64 lr;     // Link Register (Return address)
};

void UnwindStack(u64 *outStackTrace, s32 *outStackTraceSize, size_t maxStackTraceSize, u64 currFp)
{
    for (size_t i = 0; i < maxStackTraceSize; i++) {
        if (currFp == 0 || currFp % sizeof(u64) != 0)
            break;
        
        auto currTrace = reinterpret_cast<StackFrame*>(currFp); 
        outStackTrace[(*outStackTraceSize)++] = currTrace->lr;
        currFp = currTrace->fp;
    }
}

void Exit();

extern "C"
{
    void __libnx_exception_handler(ThreadExceptionDump *context)
    {
        u64 stackTrace[0x20] = {0};
        s32 stackTraceSize = 0;
        UnwindStack(stackTrace, &stackTraceSize, 0x20, context->fp.x);
        auto baseaddr = GetBaseAddress();
        auto pcstr = hos::FormatHex(context->pc.x - baseaddr);

        auto fname = "report_" + std::to_string(rand()) + ".log";
        String crashtxt = "\nGoldleaf crash report\n\n";
        crashtxt += String(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n - Current time: " + hos::GetCurrentTime() + "\n - Crash address: " + pcstr;
        std::ofstream ofs("sdmc:/" + consts::Root + "/crash/" + fname);
        if(ofs.good())
        {
            ofs << crashtxt.AsUTF8();
            ofs.close();
        }

        Exit();
    }
}

void Panic(std::string msg)
{
    // TODO: non-console panic!

    /*
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
    Exit();
    exit(0);
    */
}

void Initialize()
{
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
}

void Exit()
{
    // If Goldleaf updated itself in this session...
    if(gupdated)
    {
        romfsExit();
        fs::DeleteFile(__system_argv[0]);
        fs::RenameFile(consts::TempUpdatePath, __system_argv[0]);
    }

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
    es::Exit();
    nsExit();
    accountExit();
    ncmExit();
    nifmExit();
    pdmqryExit();
    Close();
}

int main()
{
    Initialize();

    auto renderer = pu::ui::render::Renderer::New(SDL_INIT_EVERYTHING, pu::ui::render::RendererInitOptions::RendererNoSound, pu::ui::render::RendererHardwareFlags);
    mainapp = ui::MainApplication::New(renderer);

    mainapp->Prepare();
    mainapp->ShowWithFadeIn();

    Exit();
    return 0;
}