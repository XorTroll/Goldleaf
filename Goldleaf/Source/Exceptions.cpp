#include <hos/hos_Common.hpp>

alignas(16) u8 __nx_exception_stack[0x1000];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

static u64 GetBaseAddress()
{
    u32 p;
    MemoryInfo info;

    // Find the memory region in which the function itself is stored.
    // The start of it will be the base address the homebrew was mapped to.
    svcQueryMemory(&info, &p, (u64) &GetBaseAddress);

    return info.addr;
}

extern "C"
{
    void __libnx_exception_handler(ThreadExceptionDump *context)
    {
        auto baseaddr = GetBaseAddress();
        auto pcstr = hos::FormatHex(context->pc.x - baseaddr);

        auto fname = "crash_" + pcstr + ".log";
        String crashtxt = "\nGoldleaf crash report\n\n";
        crashtxt += String(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n - Current time: " + hos::GetCurrentTime() + "\n - Crash address: " + pcstr;
        std::ofstream ofs("sdmc:/" + consts::Root + "/reports/" + fname);
        if(ofs.good())
        {
            ofs << crashtxt.AsUTF8();
            ofs.close();
        }

        Exit();
    }

    void __wrap_fatalThrow(Result rc) // This way, any kind of fatal thrown by us or libnx gets saved to a simple report, and Goldleaf simply closes itself
    {
        auto fname = "fatal_" + hos::FormatHex(rc) + ".log";
        String crashtxt = "\nGoldleaf fatal report\n\n";
        crashtxt += String(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n - Current time: " + hos::GetCurrentTime() + "\n - Fatal result: " + hos::FormatHex(rc);
        std::ofstream ofs("sdmc:/" + consts::Root + "/reports/" + fname);
        if(ofs.good())
        {
            ofs << crashtxt.AsUTF8();
            ofs.close();
        }

        Exit();
    }
}