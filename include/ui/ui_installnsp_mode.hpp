#pragma once

#include <string>
#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class InstallNSPMode : public IMode
    {
        private:
            std::string m_name;

        public:
            InstallNSPMode();

            void OnSelected() override;
            void OnNSPSelected();
            void OnDestinationSelected();
    };
}