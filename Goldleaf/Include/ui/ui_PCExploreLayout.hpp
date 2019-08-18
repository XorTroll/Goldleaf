
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
    class PCExploreLayout : public pu::ui::Layout
    {
        public:
            PCExploreLayout();
            ~PCExploreLayout();
            void UpdatePaths();
            void path_Click();
        private:
            std::vector<pu::String> names;
            std::vector<pu::String> paths;
            pu::ui::elm::Menu *pathsMenu;
            std::vector<pu::ui::elm::MenuItem*> pathItems;
    };
}