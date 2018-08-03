#pragma once

#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class InstallExtractedNSPMode : public IMode
    {
        public:
            InstallExtractedNSPMode();

            void OnSelected() override;
            void OnExtractedNSPSelected();
    };
}