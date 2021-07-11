
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

#pragma once
#include <string>
#include <switch.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include <malloc.h>
#include <cctype>
#include <cstring>
#include <cerrno>

#include <json.hpp>
#include <usbhsfs.h>
#include <curl/curl.h>
#include <pu/Plutonium>

using JSON = nlohmann::json;
using String = pu::String;

String LowerCaseString(String str);

#define R_TRY(res_expr) ({ \
    const Result _tmp_r_try_rc = res_expr; \
    if (R_FAILED(_tmp_r_try_rc)) { \
        return _tmp_r_try_rc; \
    } \
})

#define R_ASSERT(res_expr) ({ \
    const Result _tmp_r_try_rc = res_expr; \
    if (R_FAILED(_tmp_r_try_rc)) { \
        diagAbortWithResult(_tmp_r_try_rc); \
    } \
})

namespace consts {

    static const std::string Root = "switch/Goldleaf";
    static const std::string Log = Root + "/goldleaf.log";
    static const std::string Settings = Root + "/settings.json";
    static const std::string TempUpdatedNro = Root + "/update_tmp.nro";
    static const std::string Metadata = Root + "/meta";
    static const std::string Title = Root + "/title";
    static const std::string Dump = Root + "/dump";
    static const std::string DumpTemp = Root + "/dump/temp";
    static const std::string DumpUpdate = Root + "/dump/update";
    static const std::string DumpTitle = Root + "/dump/title";
    static const std::string Reports = Root + "/reports";
    static const std::string UserData = Root + "/userdata";

}

enum class ExecutableMode {
    NSO,
    NRO
};

enum class LaunchMode {
    Unknown,
    Applet,
    Application
};

enum class Language {
    English,
    Spanish,
    German,
    French,
    Italian,
    Dutch,
    Japanese,
    Portuguese,
    ChineseSimplified
};

std::string LanguageToString(Language lang);
Language StringToLanguage(std::string str);

struct ColorScheme {
    pu::ui::Color Background;
    pu::ui::Color Base;
    pu::ui::Color BaseFocus;
    pu::ui::Color Text;
};

struct Version {
    u32 major;
    u32 minor;
    s32 micro;

    String AsString();

    static inline constexpr Version MakeVersion(u32 major, u32 minor, u32 micro) {
        return { major, minor, static_cast<s32>(micro) };
    }

    static Version FromString(String ver_str);
    
    inline constexpr bool IsLower(const Version &other) const {
        if(this->major > other.major) {
            return true;
        }
        else if(this->major == other.major) {
            if(this->minor > other.minor) {
                return true;
            }
            else if(this->minor == other.minor) {
                if(this->micro > other.micro) {
                    return true;
                }
            }
        }
        return false;
    }

    inline constexpr bool IsHigher(const Version &other) const {
        return !this->IsLower(other) && !this->IsEqual(other);
    }

    inline constexpr bool IsEqual(const Version &other) const {
        return ((this->major == other.major) && (this->minor == other.minor) && (this->micro == other.micro));
    }
};

Result Initialize();
void NORETURN Close(Result rc);
void NORETURN Exit(Result rc);

inline ExecutableMode GetExecutableMode() {
    return envIsNso() ? ExecutableMode::NSO : ExecutableMode::NRO;
}

inline LaunchMode GetLaunchMode() {
    switch(appletGetAppletType()) {
        case AppletType_SystemApplication:
        case AppletType_Application: {
            return LaunchMode::Application;
        }
        case AppletType_LibraryApplet: {
            return LaunchMode::Applet;
        }
        default: {
            return LaunchMode::Unknown;
        }
    }
}

inline u64 GetCurrentApplicationId() {
    u64 app_id = 0;
    svcGetInfo(&app_id, InfoType_ProgramId, CUR_PROCESS_HANDLE, 0);
    return app_id;
}

bool HasKeyFile();
u64 GetCurrentApplicationId();
u32 RandomFromRange(u32 min, u32 max);
void EnsureDirectories();