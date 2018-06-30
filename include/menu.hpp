#pragma once

#include <array>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <switch.h>
#include "panel.hpp"

namespace menu
{
    class Menu final
    {
        public:
            PrintConsole *m_console;
            bool m_exitRequested;

            Menu(PrintConsole *console);

            void processInput(u64 keysDown);
            void updateDisplay();

            void pushPanel(std::shared_ptr<Panel> panel);
            void back();

        private:
            std::stack<std::shared_ptr<Panel>> m_panels;

            std::shared_ptr<Panel> currentPanel();
    };

    extern Menu *g_menu;
};