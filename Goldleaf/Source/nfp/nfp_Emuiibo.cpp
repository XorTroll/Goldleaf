
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

namespace nfp::emu
{
    static Service nfpemu_srv;
    static u64 nfpemu_refcnt = 0;

    bool IsEmuiiboPresent()
    {
        Handle tmph = 0;
        auto rc = smRegisterService(&tmph, EmuServiceName, false, 1);
        if(R_FAILED(rc)) return true;
        smUnregisterService(EmuServiceName);
        return false;
    }

    Result Initialize()
    {
        if(IsEmuiiboPresent()) return MAKERESULT(Module_Libnx, LibnxError_NotFound);
        atomicIncrement64(&nfpemu_refcnt);
        if(serviceIsActive(&nfpemu_srv)) return 0;
        return smGetService(&nfpemu_srv, "nfp:emu");
    }

    void Finalize()
    {
        if(atomicDecrement64(&nfpemu_refcnt) == 0) serviceClose(&nfpemu_srv);
    }
}