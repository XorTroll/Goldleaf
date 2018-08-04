#pragma once

#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class InstallExtractedNSPMode : public IMode
    {
        private:
            std::string m_name;

        public:
            InstallExtractedNSPMode();

            void OnSelected() override;
            void OnExtractedNSPSelected();
            void OnDestinationSelected();
    };
}