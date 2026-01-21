
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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
#include <vector>
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
#include <switch.h>
#include <switch-ipcext.h>
#include <glaze/glaze.hpp>
#include <usbhsfs.h>
#include <curl/curl.h>
#include <pu/Plutonium>
#include <res/res_Results.hpp>

inline constexpr size_t operator ""_KB(unsigned long long n) {
    return n * 0x400;
}

inline constexpr size_t operator ""_MB(unsigned long long n) {
    return operator ""_KB(n) * 0x400;
}

inline constexpr size_t operator ""_GB(unsigned long long n) {
    return operator ""_MB(n) * 0x400;
}

#define GLEAF_LOG_BUFFER_SIZE 0x400

#define GLEAF_LOG_WARN_PREFIX "[WARN] "
#define GLEAF_LOG_ERR_PREFIX "[ERROR] "

#define GLEAF_LOG_FMT(fmt, ...) ({ \
    auto _log_buf = GetLogBuffer(); \
    const auto _log_buf_len = snprintf(_log_buf, GLEAF_LOG_BUFFER_SIZE, fmt "\n", ##__VA_ARGS__); \
    ::LogImpl(_log_buf, _log_buf_len); \
})

#define GLEAF_WARN_FMT(fmt, ...) GLEAF_LOG_FMT(GLEAF_LOG_WARN_PREFIX fmt, ##__VA_ARGS__)
#define GLEAF_ERR_FMT(fmt, ...) GLEAF_LOG_FMT(GLEAF_LOG_ERR_PREFIX fmt, ##__VA_ARGS__)

#define GLEAF_RC_TRY(res_expr) ({ \
    const Result _tmp_rc = (res_expr); \
    if(R_FAILED(_tmp_rc)) { \
        GLEAF_WARN_FMT("Expression " #res_expr " returned 0x%X", _tmp_rc); \
        return _tmp_rc; \
    } \
})

#define GLEAF_RC_TRY_EXCEPT(res_expr, special_rc) ({ \
    const Result _tmp_rc = (res_expr); \
    if(R_FAILED(_tmp_rc)) { \
        GLEAF_WARN_FMT("Expression " #res_expr " returned 0x%X", _tmp_rc); \
        if(_tmp_rc != special_rc) { \
            return _tmp_rc; \
        } \
    } \
})

#define GLEAF_RC_UNLESS(expr, rc) ({ \
    const auto _tmp_expr = (expr); \
    if(!_tmp_expr) { \
        GLEAF_WARN_FMT("Expression " #expr " evaluated to false"); \
        return rc; \
    } \
})

#define GLEAF_RC_ASSERT(res_expr) ({ \
    const Result _tmp_rc = (res_expr); \
    if(R_FAILED(_tmp_rc)) { \
        GLEAF_ERR_FMT("Result assertion failed: '" #res_expr "' returned 0x%X", _tmp_rc); \
        diagAbortWithResult(_tmp_rc); \
    } \
})

#define GLEAF_ASSERT_FAIL(expr) ({ \
    GLEAF_ERR_FMT("Assertion failed: " #expr ""); \
    diagAbortWithResult(::rc::goldleaf::ResultAssertionFailed); \
})

#define GLEAF_ASSERT_TRUE(expr) ({ \
    const auto _tmp_expr = (expr); \
    if(!_tmp_expr) { \
        GLEAF_ERR_FMT("Assertion failed: '" #expr "'"); \
        diagAbortWithResult(::rc::goldleaf::ResultAssertionFailed); \
    } \
})

#define GLEAF_PATH_ROOT_DIR "switch/Goldleaf"

#define GLEAF_MAX_LOG_FILE_SIZE 512_KB
#define GLEAF_PATH_LOG_FILE GLEAF_PATH_ROOT_DIR "/goldleaf.log"

#define GLEAF_PATH_SETTINGS_FILE GLEAF_PATH_ROOT_DIR "/settings.json"
#define GLEAF_PATH_TEMP_UPDATE_NRO GLEAF_PATH_ROOT_DIR "/temp_update.nro"

#define GLEAF_PATH_METADATA_DIR GLEAF_PATH_ROOT_DIR "/meta"

#define GLEAF_PATH_TITLE_DIR GLEAF_PATH_ROOT_DIR "/title"

#define GLEAF_PATH_EXPORT_DIR GLEAF_PATH_ROOT_DIR "/export"
#define GLEAF_PATH_EXPORT_TEMP_DIR GLEAF_PATH_EXPORT_DIR "/temp"
#define GLEAF_PATH_EXPORT_UPDATE_DIR GLEAF_PATH_EXPORT_DIR "/update"
#define GLEAF_PATH_EXPORT_TITLE_DIR GLEAF_PATH_EXPORT_DIR "/title"

#define GLEAF_PATH_REPORTS_DIR GLEAF_PATH_ROOT_DIR "/reports"
#define GLEAF_PATH_USER_DATA_DIR GLEAF_PATH_ROOT_DIR "/userdata"

#define GLEAF_NAND_INSTALL_TEMP_DIR_NAME "gleaf-temp"
#define GLEAF_PATH_NAND_INSTALL_TEMP_DIR "Contents/" GLEAF_NAND_INSTALL_TEMP_DIR_NAME
#define GLEAF_FS_PATH_NAND_INSTALL_TEMP_DIR "@SystemContent://" GLEAF_NAND_INSTALL_TEMP_DIR_NAME

#define GLEAF_DEFINE_FLAG_ENUM(enum_type, base_type) \
inline constexpr enum_type operator|(const enum_type lhs, const enum_type rhs) { \
    return static_cast<const enum_type>(static_cast<const base_type>(lhs) | static_cast<const base_type>(rhs)); \
} \
inline constexpr enum_type operator&(const enum_type lhs, const enum_type rhs) { \
    return static_cast<const enum_type>(static_cast<const base_type>(lhs) & static_cast<const base_type>(rhs)); \
} \
inline constexpr enum_type operator~(const enum_type enm) { \
    return static_cast<const enum_type>(~static_cast<const base_type>(enm)); \
}

struct PartialJsonOptions : glz::opts {
    char indentation_char = ' ';
    u8 indentation_width = 4;
    bool comments = true;
    bool prettify = true;
    bool error_on_unknown_keys = false;
};

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
    LatinAmericanSpanish,
    German,
    French,
    Italian,
    Dutch,
    Japanese,
    Portuguese,
    Chinese,
    ChineseSimplified,
    ChineseTraditional,
    Taiwanese,
    Russian,
    Korean,

    Count,
    Auto
};

constexpr const char *LanguageNameList[] = {
    "English",
    "Español",
    "Español Latinoamericano",
    "Deutsch",
    "Français",
    "Italiano",
    "Nederlands",
    "日本語",
    "Português",
    "中文",
    "简体中文",
    "繁體中文",
    "台灣中文",
    "Русский",
    "한국어"
};
constexpr size_t LanguageNameCount = std::size(LanguageNameList);

static_assert(LanguageNameCount == static_cast<size_t>(Language::Count));

class Lock {
    private:
        Mutex mutex;

    public:
        constexpr Lock() : mutex() {}

        void lock() {
            mutexLock(&this->mutex);
        }

        void unlock() {
            mutexUnlock(&this->mutex);
        }
};

using ScopedLock = std::scoped_lock<Lock>;

class ScopeGuard {
    public:
        using Fn = std::function<void()>;

    private:
        Fn exit_fn;
        bool call;

    public:
        ScopeGuard(Fn fn) : exit_fn(fn), call(true) {}

        ~ScopeGuard() {
            if(this->call) {
                this->exit_fn();
            }
        }
        
        inline void Cancel() {
            this->call = false;
        }
};

struct ColorScheme {
    pu::ui::Color bg;
    pu::ui::Color menu_base;
    pu::ui::Color menu_base_focus;
    pu::ui::Color text;
    pu::ui::Color version_text;
    pu::ui::Color scroll_bar;
    pu::ui::Color progress_bar;
    pu::ui::Color progress_bar_bg;
    pu::ui::Color dialog_title;
    pu::ui::Color dialog_opt;
    pu::ui::Color dialog;
    pu::ui::Color dialog_over;
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
void Finalize();

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

Language GetLanguageBySystemLanguage(const SetLanguage lang);
std::string GetLanguageCode(const Language lang);
Language GetLanguageByCode(const std::string &lang_code);
std::string GetLanguageName(const Language lang);
Language GetLanguageByName(const std::string &lang_name);

std::string LowerCaseString(const std::string &str);
u32 RandomFromRange(const u32 min, const u32 max);
void EnsureDirectories();

void SetThreadName(const std::string &name);

char *GetLogBuffer();
void LogImpl(const char *log_buf, const size_t log_buf_len);
