
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

#pragma once
#include <Types.hpp>

namespace nfp::emu
{
    #define NFP_EMU_SERVICE "nfp:emu"

    constexpr SmServiceName ServiceName = smEncodeName(NFP_EMU_SERVICE);

    bool IsEmuiiboAccessible();

    Result Initialize();
    void Exit();

    namespace result
    {
        constexpr u32 Module = 352;

        constexpr Result ResultNoAmiiboLoaded = MAKERESULT(Module, 1);
        constexpr Result ResultUnableToMove = MAKERESULT(Module, 2);
        constexpr Result ResultStatusOff = MAKERESULT(Module, 3);
    }

    enum class EmulationStatus : u32
    {
        OnForever,
        OnOnce,
        Off
    };

    NX_CONSTEXPR bool StatusIsOn(EmulationStatus status)
    {
        return status != EmulationStatus::Off;
    }

    struct Version
    {
        u8 major;
        u8 minor;
        u8 micro;
        bool dev_build;
    };

    static_assert(sizeof(Version) == sizeof(u32), "Invalid nfp:emu Version!");

    Result GetCurrentAmiibo(char *out, size_t out_len);
    Result SetCustomAmiibo(const char *path);
    Result HasCustomAmiibo(bool *out_has);
    Result ResetCustomAmiibo();
    Result SetEmulationOnForever();
    Result SetEmulationOnOnce();
    Result SetEmulationOff();
    Result MoveToNextAmiibo();
    Result GetStatus(EmulationStatus *out);
    Result Refresh();
    Result GetVersion(Version *out_ver);

    struct VirtualAmiibo
    {
        std::string id;
        std::string name;
    };

    VirtualAmiibo LoadVirtualAmiibo(String path);
    String SaveAmiiboImageById(String id);
}