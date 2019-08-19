
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <string>
#include <switch.h>
#include <unistd.h>
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