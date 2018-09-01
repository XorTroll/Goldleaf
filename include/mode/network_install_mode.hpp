#pragma once

#include <switch.h>
#include "mode/mode.hpp"

namespace tin::ui
{
    class NetworkInstallMode : public IMode
    {
        private:
            void InitializeServerSocket();

        public:
            NetworkInstallMode();
            ~NetworkInstallMode();

            void OnSelected() override;
    };
}