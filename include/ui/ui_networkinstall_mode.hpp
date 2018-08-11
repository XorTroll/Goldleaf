#pragma once

#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class NetworkInstallMode : public IMode
    {
        private:

            size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len);

        public:
            NetworkInstallMode();
            ~NetworkInstallMode();

            void OnSelected() override;
    };
}