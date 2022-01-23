
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

    inline void SaveReport(const std::string &crash_log_file, const std::string &data) {
        auto sd_exp = fs::GetSdCardExplorer();
        auto crash_log_path = sd_exp->MakeAbsolute(GLEAF_PATH_REPORTS_DIR "/" + crash_log_file);
        sd_exp->WriteFile(crash_log_path, data.c_str(), data.length());
    }

}

extern "C" {

    // This way, any kind of result abort by us or libnx gets saved to a simple report, and Goldleaf simply closes itself

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