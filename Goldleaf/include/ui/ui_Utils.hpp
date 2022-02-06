
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
#include <base_Common.hpp>

namespace ui {

    void SetClipboard(const std::string &path);

    void ShowPowerTasksDialog(const std::string &title, const std::string &msg);
    std::string AskForText(const std::string &guide_text = "", const std::string &initial_text = "", const int max_len = -1);
    void HandleResult(const Result rc, const std::string &context);

    ColorScheme GenerateRandomScheme();

    constexpr u32 DefaultMenuHeight = 560;

    inline constexpr u32 ComputeDefaultMenuItemCount(const u32 menu_item_size) {
        return DefaultMenuHeight / menu_item_size;
    }

}