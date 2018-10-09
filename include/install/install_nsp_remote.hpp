#pragma once

#include <switch/services/ncm.h>
#include <string>
#include "install/install.hpp"
#include "install/remote_nsp.hpp"

namespace tin::install::nsp
{
    class RemoteNSPInstall : public Install
    {
        private:
            RemoteNSP* m_remoteNSP;

        protected:
            std::tuple<nx::ncm::ContentMeta, nx::ncm::ContentRecord> ReadCNMT() override;
            void InstallNCA(const NcmNcaId& ncaId) override;
            void InstallTicketCert() override;

        public:
            RemoteNSPInstall(FsStorageId destStorageId, bool ignoreReqFirmVersion, RemoteNSP* remoteNSP);
    };
}