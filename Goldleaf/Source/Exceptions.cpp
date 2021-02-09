#include <hos/hos_Common.hpp>

inline u64 GetBaseAddress()
{
    u32 p;
    MemoryInfo info;

    // Find the memory region in which the function itself is stored.
    // The start of it will be the base address the homebrew was mapped to.
    svcQueryMemory(&info, &p, (u64)&GetBaseAddress);

    return info.addr;
}

extern "C"
{
    alignas(16) u8 __nx_exception_stack[0x1000];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

    void __libnx_exception_handler(ThreadExceptionDump *context)
    {
        auto baseaddr = GetBaseAddress();
        auto pcstr = hos::FormatHex(context->pc.x - baseaddr);

        auto fname = "crash_" + pcstr + ".log";
        std::string crashtxt = "\nGoldleaf crash report\n\n";
        
        crashtxt += std::string(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n";
        crashtxt += "- Current time: " + hos::GetCurrentTime() + "\n";
        
        crashtxt += "- Crash address: " + pcstr + "\n";
        for(u32 i = 0; i < 29; i++)
        {
            crashtxt += " * At[" + std::to_string(i)  + "]: ";
            auto reg = context->cpu_gprs[i].x;
            if(reg > baseaddr)
            {
                crashtxt += hos::FormatHex(context->cpu_gprs[i].x - baseaddr);
            }
            crashtxt += "\n";
        }

        ErrorSystemConfig error;
        errorSystemCreate(&error, "Crash happened", crashtxt.c_str());
        errorSystemShow(&error);

        std::ofstream ofs("sdmc:/" + consts::Root + "/reports/" + fname);
        if(ofs.good())
        {
            ofs << crashtxt;
            ofs.close();
        }

        Exit(0);
    }

    NORETURN void diagAbortWithResult(Result rc) // This way, any kind of result abort by us or libnx gets saved to a simple report, and Goldleaf simply closes itself
    {
        auto fname = "fatal_" + hos::FormatHex(rc) + ".log";
        std::string crashtxt = "\nGoldleaf fatal report\n\n";
        crashtxt += std::string(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n - Current time: " + hos::GetCurrentTime() + "\n - Fatal result: " + hos::FormatHex(rc);
        
        ErrorSystemConfig error;
        errorSystemCreate(&error, "Fatal happened", crashtxt.c_str());
        errorSystemSetCode(&error, errorCodeCreateResult(rc));
        errorSystemShow(&error);
        
        std::ofstream ofs("sdmc:/" + consts::Root + "/reports/" + fname);
        if(ofs.good())
        {
            ofs << crashtxt;
            ofs.close();
        }

        Exit(rc);
    }
}