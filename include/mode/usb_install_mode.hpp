#pragma once

#include <string>
#include "mode/mode.hpp"
#include <switch/services/fs.h>

namespace tin::ui
{
    class USBInstallMode : public IMode
    {
        private:
            std::vector<std::string> m_nspNames;
            FsStorageId m_destStorageId = FsStorageId_SdCard;

        public:
            USBInstallMode();
            ~USBInstallMode();

            void OnUnwound();
            void OnSelected() override;
            void OnNSPSelected();
            void OnDestinationSelected();
    };
}