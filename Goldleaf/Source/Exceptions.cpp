
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <hos/hos_Common.hpp>
#include <fs/fs_FileSystem.hpp>

namespace {

    inline u64 GetBaseAddress() {
        u32 tmp_page_info;
        MemoryInfo mem_info;

        // Find the memory region in which the function itself is stored.
        // The start of it will be the base address the homebrew was mapped to.
        svcQueryMemory(&mem_info, &tmp_page_info, reinterpret_cast<u64>(&GetBaseAddress));

        return mem_info.addr;
    }

    void SaveReport(String crash_log_file, const std::string &data) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto crash_log_path = sd_exp->MakeAbsolute(consts::Root + "/reports/" + crash_log_file);
        sd_exp->WriteFile(crash_log_path, data.c_str(), data.length());
    }

}

extern "C" {

    // This way, any kind of result abort by us or libnx gets saved to a simple report, and Goldleaf simply closes itself

    alignas(16) u8 __nx_exception_stack[0x1000];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

    void __libnx_exception_handler(ThreadExceptionDump *exc_dump) {
        const auto base_addr = GetBaseAddress();
        auto pc_str = hos::FormatHex(exc_dump->pc.x - base_addr);

        auto crash_log_file = "crash_" + pc_str + ".log";
        std::string crash_data = "\nGoldleaf crash report\n\n";
        
        crash_data += std::string(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n";
        crash_data += "- Current time: " + hos::GetCurrentTime() + "\n";
        
        crash_data += "- Crash address: " + pc_str + "\n";
        for(u32 i = 0; i < 29; i++) {
            crash_data += " * At[" + std::to_string(i)  + "]: ";
            const auto reg = exc_dump->cpu_gprs[i].x;
            if(reg > base_addr) {
                crash_data += hos::FormatHex(reg - base_addr);
            }
            else {
                crash_data += "(...)";
            }
            crash_data += "\n";
        }

        ErrorSystemConfig error = {};
        errorSystemCreate(&error, "Crash happened", crash_data.c_str());
        errorSystemShow(&error);

        SaveReport(crash_log_file, crash_data);
        Exit(0);
    }

    NORETURN void diagAbortWithResult(Result rc) {
        auto crash_log_file = "fatal_" + hos::FormatHex(rc) + ".log";
        std::string crash_data = "\nGoldleaf fatal report\n\n";
        crash_data += std::string(" - Goldleaf version: ") + GOLDLEAF_VERSION + "\n - Current time: " + hos::GetCurrentTime() + "\n - Fatal result: " + hos::FormatHex(rc);
        
        ErrorSystemConfig error = {};
        errorSystemCreate(&error, "Fatal happened", crash_data.c_str());
        errorSystemSetCode(&error, errorCodeCreateResult(rc));
        errorSystemShow(&error);
        
        SaveReport(crash_log_file, crash_data);
        Exit(rc);
    }

}