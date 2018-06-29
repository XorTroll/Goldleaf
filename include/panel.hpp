#pragma once

#include <algorithm>
#include <switch.h>

namespace menu
{
    class Menu;

    class Panel
    {
        public:
            virtual void onBecomeActive() {}
            virtual void processInput(Menu *menu, u64 keysDown) = 0;
            virtual void updateDisplay() = 0;
    };
}