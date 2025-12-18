
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
#include <base.hpp>

namespace ui {

    constexpr size_t MaxKeyboardTextLength = 500;

    void SetClipboard(const std::string &path);

    void ShowPowerTasksDialog(const std::string &title, const std::string &msg);
    std::string ShowKeyboard(const std::string &guide_text = "", const std::string &initial_text = "", const u32 max_len = static_cast<u32>(MaxKeyboardTextLength), const size_t out_str_len = MaxKeyboardTextLength);
    void HandleResult(const Result rc, const std::string &context);

    constexpr u32 DefaultMenuHeight = 800;

    inline constexpr u32 ComputeDefaultMenuItemCount(const u32 menu_item_size) {
        return DefaultMenuHeight / menu_item_size;
    }

    enum class CommonIconKind {
        Accounts,
        Amiibo,
        Browser,
        Certificate,
        CFW,
        Directory,
        DirectoryEmpty,
        Drive,
        TextFile,
        BinaryFile,
        Copy,
        Game,
        Help,
        Image,
        Info,
        NACP,
        NAND,
        NCA,
        NRO,
        NSP,
        SdCard,
        Settings,
        Storage,
        Ticket,
        Update,
        USB,
        User,
        Pc,

        Count
    };

    void LoadCommonIcons();
    void DisposeCommonIcons();

    pu::sdl2::TextureHandle::Ref GetCommonIcon(const CommonIconKind kind);
    pu::sdl2::TextureHandle::Ref GetCommonIconForExtension(const std::string &ext);

    void ClearApplicationIcons();
    void SetApplicationIcon(const u64 app_id, pu::sdl2::TextureHandle::Ref icon);
    pu::sdl2::TextureHandle::Ref GetApplicationIcon(const u64 app_id);

    void SleepWhileRender(const u64 ns);

}
