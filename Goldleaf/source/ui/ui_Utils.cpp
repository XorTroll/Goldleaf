 
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

#include <ui/ui_Utils.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    std::string g_Clipboard;

    namespace {

        pu::sdl2::TextureHandle::Ref g_CommonIcons[static_cast<u32>(CommonIconKind::Count)] = {};

        std::unordered_map<u64, pu::sdl2::TextureHandle::Ref> g_ApplicationIcons = {};

    }

    void SetClipboard(const std::string &path) {
        g_Clipboard = path;
    }

    void ShowPowerTasksDialog(const std::string &title, const std::string &msg) {
        const auto option = g_MainApplication->DisplayDialog(title, msg, { cfg::Strings.GetString(233), cfg::Strings.GetString(232), cfg::Strings.GetString(18) }, true);
        switch(option) {
            case 0: {
                const auto option_2 = g_MainApplication->DisplayDialog(title, cfg::Strings.GetString(483), { cfg::Strings.GetString(234), cfg::Strings.GetString(18) }, true);
                if(option_2 == 0) {
                    hos::PowerOff();
                }
                break;
            }
            case 1: {
                const auto option_2 = g_MainApplication->DisplayDialog(title, cfg::Strings.GetString(484), { cfg::Strings.GetString(234), cfg::Strings.GetString(18) }, true);
                if(option_2 == 0) {
                    hos::Reboot();
                }
                break;
            }
        }
    }

    std::string ShowKeyboard(const std::string &guide_text, const std::string &initial_text, const u32 max_len, const size_t out_str_len) {
        SwkbdConfig kbd;
        auto rc = swkbdCreate(&kbd, 0);
        if(R_SUCCEEDED(rc)) {
            swkbdConfigMakePresetDefault(&kbd);
            swkbdConfigSetType(&kbd, SwkbdType_All);
            swkbdConfigSetStringLenMax(&kbd, max_len);
            if(!guide_text.empty()) {
                swkbdConfigSetGuideText(&kbd, guide_text.c_str());
            }
            if(!initial_text.empty()) {
                swkbdConfigSetInitialText(&kbd, initial_text.c_str());
            }

            auto out_text = new char[out_str_len + 1]();
            rc = swkbdShow(&kbd, out_text, out_str_len + 1);
            std::string out(out_text);
            delete[] out_text;
            swkbdClose(&kbd);

            if(R_SUCCEEDED(rc)) {
                return out;
            }
        }

        return "";
    }

    void HandleResult(const Result rc, const std::string &info) {
        if(R_FAILED(rc)) {
            std::string rc_msg = info + "\n\n" + cfg::Strings.GetString(266) + ": " + util::FormatResult(rc) + " (" + util::FormatHex(R_VALUE(rc)) + ")";

            std::string s_module_name;
            std::string s_rc_name;
            if(res::IsSpecialResult(R_VALUE(rc), s_module_name, s_rc_name)) {
                rc_msg += "\n" + cfg::Strings.GetString(264) + ": " + s_module_name + "\n" + cfg::Strings.GetString(265) + ": " + s_rc_name;
            }
            else {
                const char *module_name;
                const char *rc_name;
                if(rc::GetResultNameAny(R_VALUE(rc), module_name, rc_name)) {
                    rc_msg += "\n" + cfg::Strings.GetString(264) + ": " + module_name + "\n" + cfg::Strings.GetString(265) + ": " + rc_name;
                }
            }

            g_MainApplication->DisplayDialog(cfg::Strings.GetString(266), rc_msg, { cfg::Strings.GetString(234) }, false);
        }
    }

    void LoadCommonIcons() {
        g_CommonIcons[static_cast<u32>(CommonIconKind::Accounts)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Accounts.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Amiibo)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Amiibo.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Browser)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Browser.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Certificate)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Certificate.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::CFW)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/CFW.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Directory)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Directory.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::DirectoryEmpty)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/DirectoryEmpty.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Drive)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Drive.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::TextFile)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/TextFile.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::BinaryFile)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/BinaryFile.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Copy)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Copy.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Game)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Game.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Help)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Help.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Image)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Image.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Info)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Info.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::NACP)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/NACP.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::NAND)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/NAND.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::NCA)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/NCA.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::NRO)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/NRO.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::NSP)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/NSP.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::SdCard)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/SdCard.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Settings)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Settings.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Storage)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Storage.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Ticket)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Ticket.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Update)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Update.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::USB)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/USB.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::User)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/User.png")));
        g_CommonIcons[static_cast<u32>(CommonIconKind::Pc)] = pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(g_Settings.PathForResource("/Common/Pc.png")));
    }

    void DisposeCommonIcons() {
        for(auto &icon : g_CommonIcons) {
            icon = nullptr;
        }
    }

    pu::sdl2::TextureHandle::Ref GetCommonIcon(const CommonIconKind kind) {
        return g_CommonIcons[static_cast<u32>(kind)];
    }

    pu::sdl2::TextureHandle::Ref GetCommonIconForExtension(const std::string &ext) {
        if(ext == "nsp") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::NSP)];
        }
        else if(ext == "nro") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::NRO)];
        }
        else if(ext == "tik") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::Ticket)];
        }
        else if(ext == "cert") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::Certificate)];
        }
        else if(ext == "nca") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::NCA)];
        }
        else if(ext == "nacp") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::NACP)];
        }
        else if((ext == "jpg") || (ext == "jpeg") || (ext == "png") || (ext == "webp")) {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::Image)];
        }
        else if(ext == "bin") {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::CFW)];
        }
        else {
            return g_CommonIcons[static_cast<u32>(CommonIconKind::BinaryFile)];
        }
    }

    void ClearApplicationIcons() {
        for(auto &icon : g_ApplicationIcons) {
            icon.second.reset();
        }
        g_ApplicationIcons.clear();
    }

    void SetApplicationIcon(const u64 app_id, pu::sdl2::TextureHandle::Ref icon) {
        g_ApplicationIcons.insert({ app_id, icon });
    }

    pu::sdl2::TextureHandle::Ref GetApplicationIcon(const u64 app_id) {
        const auto it = g_ApplicationIcons.find(app_id);
        if(it != g_ApplicationIcons.end()) {
            return it->second;
        }
        return nullptr;
    }

    void SleepWhileRender(const u64 ns) {
        const u64 start = armGetSystemTick();
        while(true) {
            g_MainApplication->CallForRender();

            const u64 now = armGetSystemTick();
            if(armTicksToNs(now - start) >= ns) {
                break;
            }
        }
    }

}
