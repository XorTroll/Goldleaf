
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

#include <nfp/nfp_Emuiibo.hpp>

static Service emusrv;
static u64 emucnt = 0;

namespace nfp
{
    bool IsEmuiiboPresent()
    {
        Handle tmph = 0;
        Result rc = smRegisterService(&tmph, "nfp:emu", false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService("nfp:emu");
        return false;
    }

    Result EmuInitialize()
    {
        if(IsEmuiiboPresent()) return LibnxError_NotFound;
        atomicIncrement64(&emucnt);
        if(serviceIsActive(&emusrv)) return 0;
        return smGetService(&emusrv, "nfp:emu");
    }

    void EmuFinalize()
    {
        if(atomicDecrement64(&emucnt) == 0) serviceClose(&emusrv);
    }
}