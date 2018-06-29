#include "panel_changer.hpp"

#include <iostream>
#include "console_panel.hpp"

namespace menu
{
    namespace install_info
    {
        void displaySelectInstallLocation(menu::Menu *menu)
        {
            auto panel = std::make_shared<menu::ConsolePanel>(menu->m_console, "Display Install Info");
            auto section = panel->addSection("Install Location");
            section->addEntry("NAND", nullptr);
            section->addEntry("SD Card", nullptr);
            menu->pushPanel(panel);
        }
    }
}