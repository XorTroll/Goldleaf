#include "console_panel.hpp"

#include <iostream>
#include "menu.hpp"

namespace menu
{
    ConsolePanel::ConsolePanel()
    {
    }

    void ConsolePanel::addLine(std::string text)
    {
        this->m_lines.push_back(text);
    }

    void ConsolePanel::onBecomeActive()
    {
        consoleClear();

        for (auto &line : this->m_lines)
        {
            std::cout << line << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Press B to return." << std::endl; 
    }

    void ConsolePanel::processInput(u64 keysDown)
    {
        if (keysDown & KEY_B)
        {
            menu::g_menu->back();
        }
    }

    void ConsolePanel::updateDisplay() {}
}