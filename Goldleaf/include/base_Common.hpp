
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

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

inline constexpr size_t operator ""_KB(unsigned long long n) {
    return n * 0x400;
}

inline constexpr size_t operator ""_MB(unsigned long long n) {
    return operator ""_KB(n) * 0x400;
}

inline constexpr size_t operator ""_GB(unsigned long long n) {
    return operator ""_MB(n) * 0x400;
}

#define GLEAF_LOG_WARN_PREFIX "[WARN] "
#define GLEAF_LOG_ERR_PREFIX "[ERROR] "

#define GLEAF_LOG_FMT(fmt, ...) ({ \
    char log_buf[0x400] = {}; \
    const auto log_buf_len = sprintf(log_buf, fmt "\n", ##__VA_ARGS__); \
    ::LogImpl(log_buf, log_buf_len); \
})

#define GLEAF_WARN_FMT(fmt, ...) GLEAF_LOG_FMT(GLEAF_LOG_WARN_PREFIX fmt, ##__VA_ARGS__)
#define GLEAF_ERR_FMT(fmt, ...) GLEAF_LOG_FMT(GLEAF_LOG_ERR_PREFIX fmt, ##__VA_ARGS__)

#define GLEAF_RC_TRY(res_expr) ({ \
    const Result _tmp_rc = res_expr; \
    if (R_FAILED(_tmp_rc)) { \
        GLEAF_WARN_FMT("Expression " #res_expr " returned 0x%X", _tmp_rc); \
        return _tmp_rc; \
    } \
})

#define GLEAF_RC_UNLESS(expr, rc) ({ \
    if(!(expr)) { \
        GLEAF_WARN_FMT("Expression " #expr " evaluated to false"); \
        return rc; \
    } \
})

#define GLEAF_RC_ASSERT(res_expr) ({ \
    const Result _tmp_rc = res_expr; \
    if (R_FAILED(_tmp_rc)) { \
        GLEAF_ERR_FMT("Assertion failed: " #res_expr " returned 0x%X", _tmp_rc); \
        diagAbortWithResult(_tmp_rc); \
    } \
})

#define GLEAF_PATH_ROOT_DIR "switch/Goldleaf"

#define GLEAF_PATH_LOG_FILE GLEAF_PATH_ROOT_DIR "/goldleaf.log"
#define GLEAF_PATH_SETTINGS_FILE GLEAF_PATH_ROOT_DIR "/settings.json"
#define GLEAF_PATH_TEMP_UPDATE_NRO GLEAF_PATH_ROOT_DIR "/temp_update.nro"

#define GLEAF_PATH_METADATA_DIR GLEAF_PATH_ROOT_DIR "/meta"

#define GLEAF_PATH_TITLE_DIR GLEAF_PATH_ROOT_DIR "/title"

#define GLEAF_PATH_DUMP_DIR GLEAF_PATH_ROOT_DIR "/dump"
#define GLEAF_PATH_DUMP_TEMP_DIR GLEAF_PATH_DUMP_DIR "/temp"
#define GLEAF_PATH_DUMP_UPDATE_DIR GLEAF_PATH_DUMP_DIR "/update"
#define GLEAF_PATH_DUMP_TITLE_DIR GLEAF_PATH_DUMP_DIR "/title"

#define GLEAF_PATH_REPORTS_DIR GLEAF_PATH_ROOT_DIR "/reports"
#define GLEAF_PATH_USER_DATA_DIR GLEAF_PATH_ROOT_DIR "/userdata"

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
    ChineseSimplified,
    ChineseTraditional,
    Korean
};

struct ColorScheme {
    pu::ui::Color bg;
    pu::ui::Color base;
    pu::ui::Color base_focus;
    pu::ui::Color text;
};

struct Version {
    u32 major;
    u32 minor;
    s32 micro;

    std::string AsString();

    static inline constexpr Version MakeVersion(const u32 major, const u32 minor, const u32 micro) {
        return { major, minor, static_cast<s32>(micro) };
    }

    static Version FromString(const std::string &ver_str);
    
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
void NORETURN Close(const Result rc);
void NORETURN Exit(const Result rc);

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
            // For us, any other kinds of applets (System/OverlayApplet) are intentionally not supported
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

std::string LanguageToString(const Language lang);
Language StringToLanguage(const std::string &str);

std::string LowerCaseString(const std::string &str);
u32 RandomFromRange(const u32 min, const u32 max);
void EnsureDirectories();

// TODO: switch to LogManager for logging?

void LogImpl(const char *log_buf, const size_t log_buf_len);