#pragma once

#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class InstallNSPMode : public IMode
    {
        public:
            InstallNSPMode();

            void OnSelected() override;
            void OnNSPSelected();
    };
}