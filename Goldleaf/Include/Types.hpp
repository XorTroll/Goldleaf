
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
#include <string>
#include <switch.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ByteBuffer.hpp>
#include <json.hpp>
#include <codecvt>
#include <pu/Plutonium>

using JSON = nlohmann::json;

using String = pu::String;

String LowerCaseString(String str);

// Thanks SciresM
#define R_TRY(res_expr) \
({ \
    const Result _tmp_r_try_rc = res_expr; \
    if (R_FAILED(_tmp_r_try_rc)) { \
        return _tmp_r_try_rc; \
    } \
})

namespace consts
{
    static const std::string Root = "switch/Goldleaf";
    static const std::string Log = Root + "/goldleaf.log";
    static const std::string Settings = Root + "/settings.json";
    static const std::string TempUpdatedNro = Root + "/update_tmp.nro";
}

enum class ExecutableMode
{
    NSO,
    NRO
};

enum class LaunchMode
{
    Unknown,
    Applet,
    Application
};

enum class Storage
{
    GameCart = 2,
    NANDSystem,
    NANDUser,
    SdCard,
};

enum class Language
{
    English,
    Spanish,
    German,
    French,
    Italian,
    Dutch,
};

std::string LanguageToString(Language lang);
Language StringToLanguage(std::string str);

struct ColorScheme
{
    pu::ui::Color Background;
    pu::ui::Color Base;
    pu::ui::Color BaseFocus;
    pu::ui::Color Text;
};

struct Version
{
    u32 Major;
    u32 Minor;
    s32 Micro;

    String AsString();
    static Version MakeVersion(u32 major, u32 minor, u32 micro);
    static Version FromString(String StrVersion);
    bool IsLower(Version Other);
    bool IsHigher(Version Other);
    bool IsEqual(Version Other);
};

namespace logging
{
    template<typename ...Args>
    inline void LogFmt(std::string fmt, Args &&...args)
    {
        auto f = fopen(("sdmc:/" + consts::Log).c_str(), "a+");
        if(f)
        {
            fprintf(f, (fmt + "\n").c_str(), args...);
            fclose(f);
        }
    }
}

ExecutableMode GetExecutableMode();
LaunchMode GetLaunchMode();
u64 GetApplicationId();
bool HasKeyFile();
bool IsAtmosphere();
u64 GetCurrentApplicationId();
u32 RandomFromRange(u32 Min, u32 Max);
void EnsureDirectories();
Result Initialize();
void Close();

// Exit calls Close!
void Exit();