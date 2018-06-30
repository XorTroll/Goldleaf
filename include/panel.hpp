#pragma once

#include <algorithm>
#include <switch.h>

namespace menu
{
    class Panel
    {
        public:
            virtual void onBecomeActive() {}
            virtual void processInput(u64 keysDown) = 0;
            virtual void updateDisplay() = 0;
    };
}