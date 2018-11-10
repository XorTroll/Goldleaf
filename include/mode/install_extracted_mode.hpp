#pragma once

#include <switch.h>
#include "mode/mode.hpp"

namespace tin::ui
{
    class InstallExtractedNSPMode : public IMode
    {
        private:
            std::string m_name;
            FsStorageId m_destStorageId = FsStorageId_SdCard;
            bool m_ignoreReqFirmVersion = false;

        public:
            InstallExtractedNSPMode();

            void OnSelected() override;
            void OnExtractedNSPSelected();
            void OnDestinationSelected();
            void OnIgnoreReqFirmVersionSelected();
    };
}