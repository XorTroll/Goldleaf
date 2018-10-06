#pragma once

#include <switch.h>
#include <string>
#include "install/install.hpp"
#include "install/http_nsp.hpp"

namespace tin::install::nsp
{
    class NetworkNSPInstallTask : public Install
    {
        private:
            HTTPNSP m_httpNSP;

        protected:
            std::tuple<nx::ncm::ContentMeta, nx::ncm::ContentRecord> ReadCNMT() override;
            void InstallNCA(const NcmNcaId& ncaId) override;
            void InstallTicketCert() override;

        public:
            NetworkNSPInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion, std::string url);
    };
}