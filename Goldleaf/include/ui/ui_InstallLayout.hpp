
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
#include <ui/ui_Includes.hpp>

namespace ui {

    class InstallLayout : public pu::ui::Layout {
        private:
            pu::ui::elm::TextBlock::Ref install_text;
            pu::ui::elm::ProgressBar::Ref install_p_bar;
        public:
            InstallLayout();
            PU_SMART_CTOR(InstallLayout)

            void StartInstall(String path, fs::Explorer *exp, NcmStorageId storage_id, bool omit_confirmation = false);
    };

}