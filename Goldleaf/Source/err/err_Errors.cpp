
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

#include <err/err_Errors.hpp>
#include <errno.h>

namespace err
{
    Result Make(ErrorDescription Description)
    {
        return MAKERESULT(ErrorModule, static_cast<u32>(Description));
    }

    Result MakeErrno(int Val)
    {
        if(Val >= 0) return 0;
        return MAKERESULT(ErrnoErrorModule, (u32)errno);
    }

    Error DetermineError(Result OSError)
    {
        Error err = { 0 };
        u32 vecidx = 0;
        u32 modidx = 0;
        u32 mod = R_MODULE(OSError);
        u32 desc = R_DESCRIPTION(OSError);
        if(mod == 358) vecidx = 5;
        else if(mod == 356) vecidx = desc + 4;
        else switch(OSError)
        {
            case 0x202:
                vecidx = 1;
                break;
            case 0x234a02:
            case 0x234c02:
            case 0x234e02:
            case 0x235002:
            case 0x235202:
            case 0x235602:
            case 0x235802:
            case 0x235a02:
            case 0x235c02:
            case 0x235e02:
            case 0x236002:
            case 0x236202:
                vecidx = 2;
                break;
            case 0x236802:
            case 0x236a02:
            case 0x236c02:
            case 0x236e02:
            case 0x244402:
            case 0x244602:
            case 0x244802:
                vecidx = 3;
                break;
            default:
                vecidx = 0;
                break;
        }
        switch(mod)
        {
            case 128:
                modidx = 1;
                break;
            case 124:
                modidx = 2;
                break;
            case 5:
                modidx = 3;
                break;
            case 8:
                modidx = 4;
                break;
            case 15:
                modidx = 5;
                break;
            case 21:
                modidx = 6;
                break;
            case 16:
                modidx = 7;
                break;
            case 145:
                modidx = 8;
                break;
            case 136:
                modidx = 9;
                break;
            case 135:
                modidx = 10;
                break;
            case 26:
                modidx = 11;
                break;
            case 105:
                modidx = 12;
                break;
            case 140:
                modidx = 13;
                break;
            case 2:
                modidx = 14;
                break;
            case 356:
                modidx = 15;
                break;
            case 358:
                modidx = 16;
                break;
        }
        err.OSError = OSError;
        err.Module = Modules[modidx];
        err.Description = set::GetErrorEntry(vecidx);
        err.DescIndex = vecidx;
        return err;
    }
}