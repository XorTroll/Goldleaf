
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

#include <err/err_Result.hpp>

namespace err
{
    // These tables point to indexes of strings in the string JSONs

    static std::map<Result, u32> ResultStringTable =
    {
        { 0x202, 1 },
        { 0x234a02, 2 },
        { 0x234c02, 2 },
        { 0x234e02, 2 },
        { 0x235002, 2 },
        { 0x235202, 2 },
        { 0x235602, 2 },
        { 0x235802, 2 },
        { 0x235a02, 2 },
        { 0x235c02, 2 },
        { 0x235e02, 2 },
        { 0x236002, 2 },
        { 0x236202, 2 },
        { 0x236802, 2 },
        { 0x236a02, 2 },
        { 0x236c02, 2 },
        { 0x236e02, 2 },
        { 0x244402, 3 },
        { 0x244602, 3 },
        { 0x244802, 3 },

        { result::ResultNotEnoughSize, 5 },
        { result::ResultMetaNotFound, 6 },
        { result::ResultCNMTNotFound, 7 },
        { result::ResultTitleAlreadyInstalled, 8 },
        { result::ResultEntryAlreadyPresent, 9 },
        { result::ResultCouldNotLocateTitleContents, 10 },
        { result::ResultCouldNotBuildNSP, 11 },
        { result::ResultKeyGenMismatch, 12 },
        { result::ResultInvalidNSP, 3 },
    };

    static std::map<u32, u32> ModuleStringTable =
    {
        { 128, 1 },
        { 124, 2 },
        { 5, 3 },
        { 8, 4 },
        { 15, 5 },
        { 21, 6 },
        { 16, 7 },
        { 145, 8 },
        { 136, 9 },
        { 135, 10 },
        { 26, 11 },
        { 105, 12 },
        { 140, 13 },
        { 2, 14 },

        { result::module::Goldleaf, 15 },
        { result::module::Errno, 16 },
    };

    String GetModuleName(u32 module_id)
    {
        u32 idx = 0;
        auto find = ModuleStringTable.find(module_id);
        if(find != ModuleStringTable.end()) idx = find->second;
        return cfg::strings::Modules.GetString(idx);
    }

    String GetResultDescription(Result rc)
    {
        u32 idx = 0;
        auto find = ResultStringTable.find(rc);
        if(find != ResultStringTable.end()) idx = find->second;
        if(R_MODULE(rc) == result::module::Errno) idx = 4; 
        return cfg::strings::Results.GetString(idx);
    }
}