
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
    class PCExploreLayout : public pu::Layout
    {
        public:
            PCExploreLayout();
            ~PCExploreLayout();
            void UpdatePaths();
            void path_Click();
        private:
            std::vector<std::string> names;
            std::vector<std::string> paths;
            pu::element::Menu *pathsMenu;
            std::vector<pu::element::MenuItem*> pathItems;
    };
}