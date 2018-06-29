#include "menu.hpp"

#include <iostream>
#include <switch.h>

namespace menu
{
    Menu::Menu(PrintConsole *console) :
        m_console(console), m_exitRequested(false)
    {

    }

    void Menu::processInput(u64 keysDown)
    {
        this->currentPanel()->processInput(this, keysDown);
    }

    void Menu::updateDisplay()
    {
        this->currentPanel()->updateDisplay();
    }

    void Menu::pushPanel(std::shared_ptr<Panel> panel)
    {
        this->m_panels.push(panel);
        panel->onBecomeActive();
    }

    void Menu::back()
    {
        // Don't go back if this is the last panel in the stack. Request
        // exit instead/
        if (this->m_panels.size() > 1)
        {
            this->m_panels.pop();
            this->currentPanel()->onBecomeActive();
        }
        else
        {
            this->m_exitRequested = true;
        }
    }

    std::shared_ptr<Panel> Menu::currentPanel()
    {
        return this->m_panels.top();
    }
};