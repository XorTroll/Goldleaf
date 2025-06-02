
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
#include <ui/ui_Includes.hpp>

namespace ui {

    class WebBrowserLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::Menu::Ref opts_menu;

            void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos);
        public:
            WebBrowserLayout();
            PU_SMART_CTOR(WebBrowserLayout)

            void Refresh();
            void Reload();

            void input_DefaultKey();
            void bookmark_DefaultKey(cfg::json::WebBookmark &bmk);
    };

}
