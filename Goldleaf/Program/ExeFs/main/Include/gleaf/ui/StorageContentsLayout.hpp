
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Goldleaf>
#include <pu/Plutonium>

namespace gleaf::ui
{
    class StorageContentsLayout : public pu::Layout
    {
        public:
            StorageContentsLayout();
            void contents_Click();
            void LoadFromStorage(Storage Location);
            std::vector<horizon::Title> GetContents();
        private:
            std::vector<horizon::Title> contents;
            pu::element::TextBlock *noContentsText;
            pu::element::Menu *contentsMenu;
    };
}