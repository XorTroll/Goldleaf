
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <ui/ui_Includes.hpp>
#include <pu/Plutonium>

namespace ui
{
    class StorageContentsLayout : public pu::ui::Layout
    {
        public:
            StorageContentsLayout();
            ~StorageContentsLayout();
            void contents_Click();
            void LoadFromStorage(Storage Location);
            std::vector<hos::Title> GetContents();
        private:
            std::vector<hos::Title> contents;
            pu::ui::elm::TextBlock *noContentsText;
            pu::ui::elm::Menu *contentsMenu;
    };
}