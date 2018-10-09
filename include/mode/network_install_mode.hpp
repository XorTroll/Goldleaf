#pragma once

#include <string>
#include <vector>
#include <switch/services/fs.h>
#include "mode/mode.hpp"

namespace tin::ui
{
    class NetworkInstallMode : public IMode
    {
        private:
            std::vector<std::string> m_urls;
            FsStorageId m_destStorageId = FsStorageId_SdCard;
            void InitializeServerSocket();

        public:
            NetworkInstallMode();
            ~NetworkInstallMode();

            void OnUnwound();
            void OnSelected() override;
            void OnNSPSelected();
            void OnDestinationSelected();
    };
}