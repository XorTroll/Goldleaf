#pragma once

#include <switch.h>
#include <string>
#include "install/install.hpp"

namespace tin::install::nsp
{
    class NetworkNSPInstallTask : public IInstallTask
    {
        private:
            std::string m_url;

        public:
            NetworkNSPInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion, std::string url);

            void PrepareForInstall() override;
            void Install() override;
    };
}