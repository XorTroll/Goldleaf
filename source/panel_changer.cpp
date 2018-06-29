#include "panel_changer.hpp"

#include <iostream>
#include "console_select_panel.hpp"

namespace menu
{
    namespace main_menu
    {
        void titleInfoSelected(menu::Menu *menu)
        {
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu->m_console, "Title Info");
            auto section = panel->addSection("Install Location");
            section->addEntry("NAND", nullptr);
            section->addEntry("SD Card", nullptr);
            menu->pushPanel(panel);
        }

        void exitSelected(menu::Menu *menu)
        {
            menu->m_exitRequested = true;
        }
    }

    namespace tinfo_menu
    {

    }
}