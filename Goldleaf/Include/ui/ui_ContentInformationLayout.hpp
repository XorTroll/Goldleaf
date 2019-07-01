
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
    class ContentInformationLayout : public pu::ui::Layout
    {
        public:
            ContentInformationLayout();
            ~ContentInformationLayout();
            void UpdateElements();
            void options_Click();
            void LoadContent(hos::Title &Content);
        private:
            std::vector<hos::Title> tcontents;
            hos::TitleContents contents;
            pu::ui::elm::Menu *optionsMenu;
    };
}