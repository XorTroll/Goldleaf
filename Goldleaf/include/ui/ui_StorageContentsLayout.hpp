
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
#include <ui/ui_Includes.hpp>

namespace ui {

    class StorageContentsLayout : public pu::ui::Layout {
        private:
            NcmStorageId cur_storage_id;
            std::vector<hos::Title> contents;
            pu::ui::elm::TextBlock::Ref no_contents_text;
            pu::ui::elm::Menu::Ref contents_menu;
        public:
            StorageContentsLayout();
            PU_SMART_CTOR(StorageContentsLayout)

            void contents_DefaultKey();
            void LoadFromStorage(const NcmStorageId storage_id);

            inline void Reload() {
                this->LoadFromStorage(this->cur_storage_id);
            }

            inline std::vector<hos::Title> &GetContents() {
                return this->contents;
            }
    };

}