#pragma once

#include <string>
#include <vector>
#include "panel.hpp"

namespace menu
{
    class ConsolePanel : public menu::Panel
    {
        public:
            ConsolePanel();

            void addLine(std::string text);

            void onBecomeActive() override;
            void processInput(u64 keysDown) override;
            void updateDisplay() override;
    
        private:
            std::vector<std::string> m_lines;
    };
}