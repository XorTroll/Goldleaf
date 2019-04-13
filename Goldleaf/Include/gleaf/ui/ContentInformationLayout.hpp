
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
    class ContentInformationLayout : public pu::Layout
    {
        public:
            ContentInformationLayout();
            ~ContentInformationLayout();
            void UpdateElements();
            void options_Click();
            void LoadContent(horizon::Title &Content);
        private:
            horizon::Title content;
            std::vector<horizon::Title> subcnts;
            horizon::TitleContents contents;
            pu::element::MenuItem *baseTitleItem;
            pu::element::Menu *optionsMenu;
    };
}