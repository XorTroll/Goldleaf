#pragma once

#include <switch.h>
#include <string>
#include "install/install.hpp"
#include "install/remote_nsp.hpp"

namespace tin::install::nsp
{
    class NetworkNSPInstallTask : public Install
    {
        private:
            RemoteNSP m_remoteNSP;

        protected:
            void ReadCNMT(nx::ncm::ContentRecord* cnmtContentRecordOut, tin::util::ByteBuffer& byteBuffer) override;
            void InstallNCA(const NcmNcaId& ncaId) override;
            void InstallTicketCert() override;

        public:
            NetworkNSPInstallTask(FsStorageId destStorageId, bool ignoreReqFirmVersion, std::string url);
    };
}