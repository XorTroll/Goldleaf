
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

#pragma once
#include <string>
#include <switch.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ByteBuffer.hpp>
#include <json.hpp>
#include <pu/Plutonium>

using JSON = nlohmann::json;

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
    s32 BugFix;

    pu::String AsString();
    static Version FromString(pu::String StrVersion);
    bool IsLower(Version Other);
    bool IsHigher(Version Other);
    bool IsEqual(Version Other);
};

static const std::string GoldleafDir = "switch/Goldleaf";

ExecutableMode GetExecutableMode();
LaunchMode GetLaunchMode();
pu::String GetVersion();
u64 GetApplicationId();
bool HasKeyFile();
bool IsAtmosphere();
bool IsReiNX();
bool IsSXOS();
u64 GetCurrentApplicationId();
u32 RandomFromRange(u32 Min, u32 Max);
void EnsureDirectories();