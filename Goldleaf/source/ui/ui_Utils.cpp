 
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

#include <ui/ui_Utils.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    std::string g_Clipboard;

    namespace {

        inline constexpr u8 VariateChannelImpl(const u8 input, const u8 v) {
            const auto tmp = static_cast<u32>(input + v);
            if(tmp > 0xFF) {
                return 0xFF;
            }
            return static_cast<u8>(tmp);
        }

        inline pu::ui::Color GenerateColorVariation(const pu::ui::Color clr, const u8 min_v, const u8 max_v) {
            const auto v = static_cast<u8>(RandomFromRange(min_v, max_v));
            return { VariateChannelImpl(clr.r, v), VariateChannelImpl(clr.g, v), VariateChannelImpl(clr.b, v), clr.a };
        }

        constexpr pu::ui::Color TextLight = { 225, 225, 225, 0xFF };
        constexpr pu::ui::Color TextDark = { 15, 15, 15, 0xFF };

    }

    void SetClipboard(const std::string &path) {
        g_Clipboard = path;
    }

    void ShowPowerTasksDialog(const std::string &title, const std::string &msg) {
        const auto option = g_MainApplication->CreateShowDialog(title, msg, { cfg::strings::Main.GetString(233), cfg::strings::Main.GetString(232), cfg::strings::Main.GetString(18) }, true);
        switch(option) {
            case 0: {
                hos::PowerOff();
                break;
            }
            case 1: {
                hos::Reboot();
                break;
            }
        }
    }

    std::string AskForText(const std::string &guide_text, const std::string &initial_text, int max_len) {
        SwkbdConfig kbd;
        auto rc = swkbdCreate(&kbd, 0);
        if(R_SUCCEEDED(rc)) {
            swkbdConfigMakePresetDefault(&kbd);
            if(max_len > 0) {
                swkbdConfigSetStringLenMax(&kbd, static_cast<u32>(max_len));
            }

            if(!guide_text.empty()) {
                swkbdConfigSetGuideText(&kbd, guide_text.c_str());
            }

            if(!initial_text.empty()) {
                swkbdConfigSetInitialText(&kbd, initial_text.c_str());
            }

            char out_text[FS_MAX_PATH] = {};
            rc = swkbdShow(&kbd, out_text, sizeof(out_text));
            if(R_SUCCEEDED(rc)) {
                return out_text;
            }
            swkbdClose(&kbd);
        }

        return "";
    }

    void HandleResult(const Result rc, const std::string &info) {
        if(R_FAILED(rc)) {
            const auto mod_info = err::GetModuleName(R_MODULE(rc)) + " (" + std::to_string(R_MODULE(rc)) + ")";
            const auto desc_info = err::GetResultDescription(rc) + " (" + std::to_string(R_DESCRIPTION(rc)) + ")";
            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(266), info + "\n\n" + cfg::strings::Main.GetString(266) + ": " + hos::FormatResult(rc) + " (" + hos::FormatHex(rc) + ")\n" + cfg::strings::Main.GetString(264) + ": " + mod_info + "\n" + cfg::strings::Main.GetString(265) + ": " + desc_info + "", { cfg::strings::Main.GetString(234) }, false);
        }
    }

    ColorScheme GenerateRandomScheme() {
        ColorScheme scheme = {};
        const auto r = static_cast<u8>(RandomFromRange(0, 0xFF));
        const auto g = static_cast<u8>(RandomFromRange(0, 0xFF));
        const auto b = static_cast<u8>(RandomFromRange(0, 0xFF));

        const pu::ui::Color clr = { r, g, b, 0xFF };
        scheme.base = clr;
        scheme.bg = GenerateColorVariation(clr, 30, 50);
        scheme.base_focus = GenerateColorVariation(clr, 20, 30);

        const auto av = (r + g + b) / 3;
        if((2 * av) < 0xFF) {
            scheme.text = TextLight;
        }
        else {
            scheme.text = TextDark;
        }

        return scheme;
    }

}